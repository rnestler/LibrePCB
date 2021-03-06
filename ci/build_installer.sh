#!/usr/bin/env bash

# set shell settings (see https://sipb.mit.edu/doc/safe-shell/)
set -euv -o pipefail

if [ "$OS" = "linux" ]
then
  EXECUTABLE_EXT="run"
elif [ "$OS" = "mac" ]
then
  EXECUTABLE_EXT="dmg"
elif [ "$OS" = "windows" ]
then
  EXECUTABLE_EXT="exe"
fi
TARGET="$OS-$ARCH"
./dist/installer/update_metadata.sh "$TARGET" "0.1.0"  # TODO: How to determine version number?
PACKAGES_DIR="./artifacts/installer_packages/$TARGET"
mkdir -p $PACKAGES_DIR/librepcb.nightly.app/data/nightly
cp -r ./dist/installer/output/packages/. $PACKAGES_DIR/
cp -r ./build/install/opt/. $PACKAGES_DIR/librepcb.nightly.app/data/nightly/
binarycreator --online-only -c ./dist/installer/output/config/config.xml -p $PACKAGES_DIR \
              ./artifacts/nightly_builds/librepcb-installer-nightly-$TARGET.$EXECUTABLE_EXT
