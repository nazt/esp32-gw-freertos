#!/bin/bash

echo "Installing Python Wheel ..."
pip install wheel > /dev/null 2>&1

echo "Installing PlatformIO ..."
# pip install -U platformio > /dev/null 2>&1
pip install -U https://github.com/platformio/platformio-core/archive/develop.zip > /dev/null 2>&1

echo "GITHUB_REPOSITORY=$GITHUB_REPOSITORY"
echo "GITHUB_WORKSPACE=$GITHUB_WORKSPACE"


echo "PlatformIO has been installed"
echo ""
