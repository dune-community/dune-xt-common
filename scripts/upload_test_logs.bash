#! /bin/bash

BRANCH=PR_${TRAVIS_PULL_REQUEST}_to_${TRAVIS_BRANCH}
if [ "x${TRAVIS_PULL_REQUEST}" == "xfalse" ] ; then
    BRANCH=${TRAVIS_BRANCH}
fi
cd
rm -rf logs
git clone https://github.com/wwu-numerik/dune-stuff-testlogs.git logs
cd logs
git checkout -B ${BRANCH}
DIR=${CXX_COMPILER}__wo_${MODULES_TO_DELETE// /_}
rm -rf ${DIR}
mkdir ${DIR}
rsync -a ${DUNE_BUILD_DIR}/dune-stuff/dune/stuff/test/*xml ${DIR}
printenv | grep -v GH_TOKEN | sort > ${DIR}/env
git add .
git config user.name "Travis-CI"
git config user.email "travis@travis-ci.org"
git commit -m "Testlogs for Job ${TRAVIS_JOB_NUMBER} - ${TRAVIS_COMMIT_RANGE}"
git push -u https://${GH_TOKEN}@github.com/wwu-numerik/dune-stuff-testlogs.git ${BRANCH} > /dev/null 2>&1
