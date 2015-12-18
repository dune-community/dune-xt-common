#! /bin/bash
#
# This file is part of the dune-stuff project:
#   https://github.com/wwu-numerik/dune-stuff
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2015)
#   Rene Milk       (2015)

BRANCH=${TRAVIS_BRANCH}
if [ "x${TRAVIS_PULL_REQUEST}" != "xfalse" ] ; then
    BRANCH=PR_${TRAVIS_PULL_REQUEST}_to_${BRANCH}
fi
cd
rm -rf logs
git clone https://github.com/wwu-numerik/dune-stuff-testlogs.git logs
cd logs
git checkout -B ${BRANCH}
DIR=${CXX}
if [ "x${MODULES_TO_DELETE}" != "x" ] ; then
    DIR=${DIR}__wo_${MODULES_TO_DELETE// /_}
fi
rm -rf ${DIR}
mkdir ${DIR}
rsync -a ${DUNE_BUILD_DIR}/dune-stuff/dune/stuff/test/*xml ${DIR}
printenv | grep -v GH_TOKEN | sort > ${DIR}/env
git add --all .
git config user.name "Travis-CI"
git config user.email "travis@travis-ci.org"
git commit -m "Testlogs for Job ${TRAVIS_JOB_NUMBER} - ${TRAVIS_COMMIT_RANGE}"
git push -u https://${GH_TOKEN}@github.com/wwu-numerik/dune-stuff-testlogs.git ${BRANCH}
