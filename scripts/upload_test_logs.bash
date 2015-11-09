#! /bin/bash
set -u
set -x

BRANCH=${TRAVIS_BRANCH}
if [ x${TRAVIS_PULL_REQUEST} -ne xfalse ] ; then
    BRANCH=PR_${TRAVIS_PULL_REQUEST}_to_${TRAVIS_BRANCH}
fi
cd
rm -rf logs
git clone https://${GH_TOKEN}@github.com/wwu-numerik/dune-stuff-testlogs.git logs
cd logs
git checkout ${BRANCH}
DIR=${CXX_COMPILER}__wo_${MODULES_TO_DELETE// /_}
rm -rf ${DIR}
rsync -a ${DUNE_BUILD_DIR}/dune/stuff/test/*xml ${DIR}
printenv | sort > ${DIR}/env
git add .
git config user.name "Travis-CI"
git config user.email "travis@travis-ci.org"
git commit -m "Testlogs for Job ${TRAVIS_JOB_NUMBER} - ${TRAVIS_COMMIT_RANGE}"
git push origin ${BRANCH}
