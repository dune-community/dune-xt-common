#!/usr/bin/env python3
import logging
import os
import pickle
import json
import sys
import pprint
import subprocess
import time
from contextlib import contextmanager
import binpacking
from multiprocessing import Pool, cpu_count
from collections import OrderedDict

MAXTIME = 23*60
pickle_file = 'totals.pickle'
PROTOCOL = 0

@contextmanager
def elapsed_timer():
    clock = time.time
    start = clock()
    elapser = lambda: clock() - start
    yield lambda: elapser()
    end = clock()
    elapser = lambda: end-start


def _dump(obj, fn):
    json.dump(obj, open(fn, 'wt'), sort_keys=True)


def _load(fn):
    try:
        return json.load(open(fn, 'rt'))
    except:
        return pickle.load(open(fn, 'rb'))


def _compile(binary):
    with elapsed_timer() as timer:
        try:
            _ = subprocess.check_output(['cmake', '--build', '.', '--', '-j1', binary], universal_newlines=True, stderr=subprocess.STDOUT)
        except subprocess.CalledProcessError as cpe:
            if 'Timeout' not in cpe.output:
                raise cpe
            print('Timeout in compile {}'.format(binary))
        return timer()


def _run_tests(tpl):
    binary, teststrings = tpl
    testtimes = 0
    for test in teststrings.split(';'):
        with elapsed_timer() as timer:
            try:
                _ = subprocess.check_output(['ctest', '-j1', '-N', '-R', test], universal_newlines=True,
                                            stderr=subprocess.STDOUT)
            except subprocess.CalledProcessError as cpe:
                if 'Timeout' not in cpe.output:
                    raise cpe
                # be pessimistic and double the timeout value as time for this run
                testtimes += timer()
                print('Timeout in {} from {}'.format(test, binary))
            testtimes += timer()
    return testtimes


def _redo(processes, keys, *args):
    try:
        with Pool(processes=processes) as pool:
            result = pool.map(*args)
        return {k: v for k,v in zip(keys, result)}
    except subprocess.CalledProcessError as cpe:
        logging.error('*'*79)
        logging.error(cpe.stdout)
        logging.error(cpe.stderr)
        logging.error('*' * 79)
        raise cpe

def do_timings(builddir, pickledir, binaries, testnames, processes, headerlibs):
    os.chdir(builddir)
    testlimit = -1

    binaries = binaries[:testlimit]
    headerlibs = headerlibs[:testlimit]
    targets = binaries+headerlibs
    compiles_fn = os.path.join(pickledir, 'compiles_' + pickle_file)
    try:
        compiles = _load(compiles_fn)
        if set(compiles.keys()) != set(targets):
            logging.error('redoing compiles due to mismatched binaries')
            logging.error('Removed: {}'.format(pprint.pformat(set(compiles.keys()) - set(targets))))
            logging.error('New: {}'.format(pprint.pformat(set(targets) - set(compiles.keys()))))
            compiles = _redo(processes, targets, _compile, targets)
    except FileNotFoundError:
        logging.error('redoing compiles due to missing pickle')
        compiles = _redo(processes, targets, _compile, targets)
    _dump(compiles, compiles_fn)

    testnames = testnames[:testlimit]
    testruns_fn = os.path.join(pickledir, 'testruns_' + pickle_file)
    try:
        loaded_testnames, testruns = _load(testruns_fn)
        if set(compiles.keys()) != set(targets) or loaded_testnames != testnames:
            logging.error('redoing tests due to mismatched binaries/testnames')
            logging.error('Removed: {}'.format(pprint.pformat([f for f in loaded_testnames if f not in set(testnames)])))
            logging.error('New: {}'.format(pprint.pformat([n for n in testnames if n not in set(loaded_testnames)])))
            testruns = _redo(processes, binaries, _run_tests, zip(binaries, testnames))
    except FileNotFoundError:
        logging.error('redoing tests due to missing pickle')
        testruns = _redo(processes, binaries, _run_tests, zip(binaries, testnames))
    _dump((testnames, testruns), testruns_fn)

    totals = {n: compiles[n]+testruns[n] for n in binaries}
    # add totals for headerlib compiles that do not have associated testruns
    totals.update({n: compiles[n] for n in headerlibs})
    _dump(totals, os.path.join(pickledir, pickle_file))
    # print('totals')
    # pprint.pprint(totals)
    return totals


# list comes with a leading empty entry
all_testnames = open(sys.argv[4], 'rt').read().split('/')[1:]
builddir = sys.argv[1]
testdir = sys.argv[2]
cmake_outfile = os.path.join(builddir, 'builder_definitions.cmake')
binaries = open(sys.argv[3], 'rt').read().split(';')
headerlibs = open(sys.argv[5], 'rt').read().split(';')
try:
    bincount = int(sys.argv[6])
except:
    bincount = 13
logging.basicConfig(level=logging.DEBUG)
testname_map = {b: t.split(';') for b,t in zip(binaries, all_testnames)}
processes = cpu_count()

totals = do_timings(builddir, testdir, binaries, all_testnames, processes, headerlibs)

#bins = binpacking.to_constant_volume(totals, MAXTIME)
bins = binpacking.to_constant_bin_number(totals, bincount)
#for idx, bin in enumerate(bins):
    #pprint('Bin {} vol: {}'.format(idx, sum(bin.values())))
    #pprint.pprint(bin)
from statistics import mean, stdev
vols = [sum(bi.values()) for bi in bins]
norm = 100/MAXTIME
print('Generated {} bins.\nRelative volumes:\n\t\tMin {:.2f}%\n\t\tMax {:.2f}%\n\t\tAvg {:.2f}%\n'.format(
    len(bins), min(vols)*norm, max(vols)*norm, mean(vols)*norm, stdev(vols)))

with open(cmake_outfile, 'wt') as out:
    out.write('set(DXT_BIN_COUNT "{}" CACHE STRING "number of bins for test targets" )\n'.format(len(bins)))
    for idx, bin in enumerate(bins):
        out.write('add_custom_target(test_binaries_builder_{} DEPENDS {})\n'.format(idx, ' '.join(sorted(bin.keys()))))
        for binary in sorted(bin.keys()):
            if binary not in headerlibs:
                for testname in testname_map[binary]:
                    out.write('set_tests_properties({} PROPERTIES LABELS "builder_{}")\n'.format(testname, idx))
