#! /bin/bash
#
# This file is part of the dune-xt-common project:
#   https://github.com/dune-community/dune-xt-common
# The copyright lies with the authors of this file (see below).
# License: BSD 2-Clause License (http://opensource.org/licenses/BSD-2-Clause)
# Authors:
#   Felix Schindler (2015 - 2016)
#   Rene Milk       (2015)
#   Tobias Leibner  (2015)

BRANCH=${TRAVIS_BRANCH}
if [ "x${TRAVIS_PULL_REQUEST}" != "xfalse" ] ; then
    BRANCH=PR_${TRAVIS_PULL_REQUEST}_to_${BRANCH}
fi
cd
rm -rf logs
TESTLOGS_URL=https://github.com/wwu-numerik/dune-stuff-testlogs.git
git clone ${TESTLOGS_URL} logs
cd logs
# check if branch exists, see http://stackoverflow.com/questions/8223906/how-to-check-if-remote-branch-exists-on-a-given-remote-repository
if [ `git ls-remote --heads ${TESTLOGS_URL} ${BRANCH} | wc -l` -ne 0 ] ; then
git checkout ${BRANCH}
else
git checkout -b ${BRANCH}
fi
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
