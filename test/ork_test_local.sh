
# Why not when it is so easy
sudo apt-get install -y g++
sudo apt-get install -y ninja-build

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
cmake -G 'CodeLite - Ninja' "$REPO_ROOT/ork/test"

cd "$( dirname "${BASH_SOURCE[0]}" )"

