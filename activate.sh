#!/bin/sh
export PATH=$PWD/bin:$PATH
exec "${@:-$SHELL}"
