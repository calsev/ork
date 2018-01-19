
cd ../..
REPO_ROOT=`pwd`
cd "$( dirname "${BASH_SOURCE[0]}" )"
ORK_UNICODE='ON'

if [ ! -d "$REPO_ROOT/ork_test" ]; then
  mkdir "$REPO_ROOT/ork_test"
fi
if [ ! -d "$REPO_ROOT/ork_test/bin" ]; then
  mkdir "$REPO_ROOT/ork_test/bin"
fi
if [ ! -d "$REPO_ROOT/ork_test/build" ]; then
  mkdir "$REPO_ROOT/ork_test/build"
fi

cd "$REPO_ROOT/ork_test/build"


cd "$( dirname "${BASH_SOURCE[0]}" )"

