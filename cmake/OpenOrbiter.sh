#!/bin/sh
# not upstream: the "OpenOrbiter" shortcut (CPACK_PACKAGE_EXECUTABLES) for Linux; Orbiter runs from its own folder like the Windows shortcut's "Start in"
cd "$(dirname "$(readlink -f "$0")")" || exit 1
exec ./Orbiter "$@"
