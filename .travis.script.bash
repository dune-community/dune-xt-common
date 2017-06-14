#!/bin/bash
#
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# Copyright 2009-2017 dune-xt-common developers and contributors. All rights reserved.
# License: Dual licensed as BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
#      or  GPL-2.0+ (http://opensource.org/licenses/gpl-license)
#          with "runtime exception" (http://www.dune-project.org/license.html)
# Authors:
#   Felix Schindler (2017)
#   Rene Milk       (2017)

# ****** THIS FILE IS AUTOGENERATED, DO NOT EDIT **********

set -e
set -x

${SRC_DCTRL} ${BLD} --only=${MY_MODULE} configure
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD}
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} test_binaries
if [[ "${CC}" == "gcc"* ]] ; then
    lcov -q --gcov-tool ${GCOV} -b ${SUPERDIR}/${MY_MODULE} -d ${DUNE_BUILD_DIR}/${MY_MODULE} -c -o ${HOME}/baseline.lcov --no-external --initial
fi

${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} test
${SUPERDIR}/.travis/init_sshkey.bash ${encrypted_862ca47045d1_key} ${encrypted_862ca47045d1_iv} keys/dune-community/dune-xt-common-testlogs
# retry this step becuase of the implicated race condition in cloning and pushing with multiple builder running in parallel
${SUPERDIR}/scripts/bash/travis_upload_test_logs.bash ${DUNE_BUILD_DIR}/${MY_MODULE}/dune/xt/*/test/

${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} headercheck
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} install | grep -v "Installing"
${SRC_DCTRL} ${BLD} --only=${MY_MODULE} bexec ${BUILD_CMD} package_source

# ****** THIS FILE IS AUTOGENERATED, DO NOT EDIT **********