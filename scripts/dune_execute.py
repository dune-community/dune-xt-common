#!/usr/bin/env python

# This is a modified version of dune_execute.py from dune-testtools.
# It adds the --gtest_output option to the call.

import os.path
import subprocess
import sys

from dune.testtools.wrapper.argumentparser import get_args
from dune.testtools.parser import parse_ini_file

def call(executable, inifile=None, *additional_args):
    # If we have an inifile, parse it and look for special keys that modify the execution
    command = ["./" + executable]
    if inifile:
        iniargument = inifile
        iniinfo = parse_ini_file(inifile)
        if "__inifile_optionkey" in iniinfo:
            command.append(iniinfo["__inifile_optionkey"])
        command.append(iniargument)
    for arg in additional_args:
        command.append(arg)
    return subprocess.call(command)

# Parse the given arguments
args = get_args()
sys.exit(call(args["exec"], args["ini"], "--gtest_output=xml:" + os.path.splitext(os.path.basename(args["exec"]))[0] + "_" + os.path.splitext(os.path.basename(args["ini"]))[0] + ".xml"))
