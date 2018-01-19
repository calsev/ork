This script runs the ork test suite locally.
Because ork builds and uses sibling dependencies, this requires some environment setup.
The environment created here must be reproduced in appveyor for automated tests to work.
This script assumes a local repo setup exists that is functionally equivalent to that below.

repo_root
|-boost_141 (git repo)
|-catch (git repo)
|-glm (git repo)
|-json (git repo)
|-ork (git repo)
|-ork_test_141 (created here)
  |-bin (created here)
  |-build (created here)
|-pugi (git repo)
|-yaml (git repo)

