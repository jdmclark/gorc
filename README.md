# Gorc

[![Build Status](https://travis-ci.org/jdmclark/gorc.svg?branch=develop)](https://travis-ci.org/jdmclark/gorc) [![Coverage Status](https://coveralls.io/repos/jdmclark/gorc/badge.svg?branch=develop&service=github)](https://coveralls.io/github/jdmclark/gorc?branch=develop) [![Coverity Scan Build Status](https://scan.coverity.com/projects/2223/badge.svg)](https://scan.coverity.com/projects/2223)

Gorc is an early-development Dark Forces II game engine recreation. The goal of the Gorc project is to replace the original game executable, allowing the original singleplayer and multiplayer levels to be played on new platforms.

## Developing

### Dependencies

* Standard C++11 compiler (Clang 3.3, GCC 4.8)
* GNU Make
* SFML 2.1
* [Boost](http://boost.org)
* [Gcovr 3.2](http://gcovr.com)
* Dark Forces II

### Quick Start

#### Bootstrapping

The Gorc build system must be built first. This step only needs to be done once.

* Run `make bootstrap` from the project root.

The Gorc build and test system is now located in the `bin` subdirectory of the project root. For
convenience, you may run `activate.sh` to launch a new shell with the build tools in PATH.

#### Building

* Build the entire project: run `boc` from the project root.

or

* Build a specific component: run `boc` from the component source directory.

### Testing

* Run all tests: run `boc test` from the project root.

or

* Run a subset of tests: run `boc test` from a component source directory.

or

* Run a specific test: run `boc` from the test directory.

### Test Coverage

Gorc uses the `gcov` coverage tool. The Gorc build system provides special options for coverage
report generation. Use the following command to quickly generate a complete coverage report.

* Run `boc clean build test coverage-report --type coverage` from the project root.

The Gorc project has adopted a 100% line coverage policy to encourage the development of highly
testable code. The test coverage report must show 100% line coverage before any code changes will
be accepted.

#### Running

Before running Gorc, you must copy your Dark Forces II original `episode` and `resource`
directories into the `game` directory in the root of this repository. Gorc will not work without
these directories. This step only needs to be done once.

After the files have been copied, you can run the Gorc client from the command line. Gorc does not
currently allow you to play through the levels normally. You must specify an episode GOB and a level
filename manually.

For example, to play the first level you would run:

  pkg/bin/client --episode game/episode/jk1.gob --level 01narshadda.jkl

Other episodes and levels can be played by using appropriate `--episode` and `--level` options.

If you do not know the level filenames inside an episode, you can use the included `gob` utility to
list them:

  pkg/bin/gob --file game/episode/jk1.gob --extract episode.jk
