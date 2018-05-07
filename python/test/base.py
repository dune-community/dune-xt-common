# ~~~
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2018 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Rene Milk (2018)
# ~~~

import pytest
from dune.xt.common.test import load_all_submodule


def test_load_all():
    import dune.xt.common as xtc
    load_all_submodule(xtc)


def test_empty():
    from dune.xt.empty import Dog, Pet, Terrier

    dog = Dog('Susi')
    pet = Pet('Bello')
    ter = Terrier()

    assert ter.getName() == 'Berti'
    assert pet.getName() == 'Bello'
    assert ter.bark() == 'woof!'


def test_logging():
    import dune.xt.common.logging as lg
    lg.create(lg.log_max);
    lg.info('log info test')
    lg.error('log error test')
    lg.debug('log debug test')


def test_timings():
    from dune.xt.common.timings import timings
    timings.reset()
    timings.start("foo.bar")
    timings.stop()
    timings.output_simple()
