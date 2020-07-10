#!/bin/bash

set -e

if [ ! -z "$TRAVIS_BUILD_DIR" ]; then
	export GITHUB_WORKSPACE="$TRAVIS_BUILD_DIR"
	export GITHUB_REPOSITORY="$TRAVIS_REPO_SLUG"
elif [ -z "$GITHUB_WORKSPACE" ]; then
	export GITHUB_WORKSPACE="$PWD"
fi

TARGET_PLATFORM="$1"
CHUNK_INDEX=$2
CHUNKS_CNT=$3

BUILD_PIO=1

if [ "$BUILD_PIO" -eq 0 ]; then
	# ArduinoIDE Test
	echo "1"
else
	# PlatformIO Test
	source ./.github/scripts/install-platformio.sh 
	mkdir -p $HOME/output
 	platformio ci --lib="." --lib="lib/*" --build-dir="$HOME/output" --keep-build-dir --project-conf=./platformio.ini .
	
	# mkdir -p $HOME/ibeacon-scanner
	# platformio ci --lib="beacon32/lib/*"  --build-dir="$HOME/beacon32" --keep-build-dir  --project-conf=./beacon32/platformio.ini beacon32
	# platformio ci --lib="ibeacon-scanner/lib/*"  --build-dir="$HOME/ibeacon-scanner" --keep-build-dir  --project-conf=./ibeacon-scanner/platformio.ini  ibeacon-scanner
	cp -Rv /Users/runner/beacon32/.pio/build/esp32dev/firmware.bin "$HOME/fw.bin"
	# cp -Rv /Users/runner/ibeacon-scanner/.pio/build/esp32dev/firmware.bin "$HOME/ibeacon-scanner.bin" 
fi