#!/bin/sh
$EXTRACTRC $(find conf/ -name "*.ui" -o -name "*.kcfg") >> rc_okular_ghostview.cpp || exit 11
$XGETTEXT $(find . -name "*.cpp" -o -name "*.h") -o $podir/okular_ghostview.pot
rm -f rc_okular_ghostview.cpp
