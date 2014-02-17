Gorc
====

Description
-----------

Early-development Dark Forces II game engine recreation.

Instructions
------------

* Create symbolic links to the `resource` and `episode` directories inside the `game` subdirectory.
* Optional: Rip the CD audio to the `game/restricted/music/1` and `game/restricted/music/2` subdirectories in OGG format.
* Run `make` from the project root.

Controls
--------

* W,A,S,D: Move player
* Space: Jump
* Left mouse: Shoot bryarbolt
* E: Activate surface/thing
* G: Give all inventory items
* F1: Activate fieldlight (requires batteries)
* R: Teleport to next walkplayer.
* Esc: Quit

Requirements
------------

* Standard C++11 compiler (Clang 3.3, GCC 4.8)
* GNU Make
* Flex 2.5
* Bison 2.5
* SFML 2.1
* [Boost](http://boost.org)
* Dark Forces II