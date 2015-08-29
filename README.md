# Gorc

[![Build Status](https://travis-ci.org/jdmclark/gorc.svg?branch=next)](https://travis-ci.org/jdmclark/gorc) [![Coverity Scan Build Status](https://scan.coverity.com/projects/2223/badge.svg)](https://scan.coverity.com/projects/2223)

Gorc is an early-development Dark Forces II game engine recreation. The goal of the Gorc project is to replace the original game executable, allowing the original singleplayer and multiplayer levels to be played on new platforms.

## Developing

### Dependencies

* Standard C++11 compiler (Clang 3.3, GCC 4.8)
* GNU Make
* Flex 2.5
* Bison 2.5
* SFML 2.1
* [Boost](http://boost.org)
* [Gcovr 3.2](http://gcovr.com)
* Dark Forces II

### Building

* Build the entire project: run `make` from the project root.

or

* Build a specific component: run `make` from the component source directory.

### Testing

* Run all tests: run `make test` from the project root.

or

* Run a subset of tests: run `make test` from a component source directory.

or

* Run a specific test: run `make` from the test directory.

### Test Coverage

Gorc uses the `gcov` coverage tool. The Gorc build system provides special targets for coverage report generation. Use the following steps to generate a full coverage report.

* Build the entire project with `gcov` instrumentation: run `make BUILD_CONFIG=test-coverage` from the project root.

* Run all tests with `gcov` instrumentation: run `make test` from the project root.

* Generate a test coverage report: run `make test-coverage-report` from the project root.

The Gorc project has adopted a 100% line coverage policy to encourage the development of highly testable code. The test coverage report must show 100% line coverage before any code changes will be accepted.
