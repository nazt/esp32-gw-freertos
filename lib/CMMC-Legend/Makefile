SHELL := /bin/bash

CWD=$(shell pwd)
VERSION=$(shell node -pe "require('./library.json').version")
SEMVER=`git-semver.sh`
SPIFFS=$(HOME)/.platformio/packages/tool-mkspiffs/mkspiffs
ESPTOOL=$(HOME)/Library/Arduino15/packages/esp8266/tools/esptool/0.4.13/esptool 


define release
	test -n "$(1)" 
	git-semver.sh 
	@sed -i 's/version=.*/version=$1/' library.properties
	echo "CURRENT_VERSION=${VERSION}" 
	node -e "\
		var j = require('./library.json');\
		j.version = '$(1)';\
		var s = JSON.stringify(j, null, 4) + '\n';\
		require('fs').writeFileSync('./library.json', s);"
	@git commit -am "Update meta version code for V$1"
	echo "/* Generated file, do not edit. */" > src/version.h
	echo "#define LEGEND_APP_VERSION \"$1\"" >> src/version.h
	echo "#define LEGEND_APP_VERSION_DATE \"`git log -n 1 --format=%ai`\"" >> src/version.h
	@git commit -am "Update library version for V$1"
	@git tag -a "v$(1)" -m "release $(1)"
	git push --tags origin HEAD:master
	@echo "releasing V=$1"
endef

define commit
	echo "semver=${SEMVER}"
	echo "/* Generated file, do not edit. */" > src/version.h
	echo "#define LEGEND_APP_VERSION \"${SEMVER}\"" >> src/version.h
	echo "#define LEGEND_APP_VERSION_DATE \"`git log -n 1 --format=%ai`\"" >> src/version.h
	@git commit -am "update ${SEMVER}"
endef

release:
	@$(call release,$V) 
update-version:
	@$(call commit,$m) 
version:
	echo "CURRENT_VERSION=${VERSION}"
spiffs:
	echo "CURRENT_VERSION=${VERSION}"
	# ./rebuild-spiffs.sh
	rm data/index.htm
	$(SPIFFS) -c data -p 256 -b 8192 -s 1028096 binaries/spiffs-1M.bin 
build-spiffs: 
	./rebuild-spiffs.sh
build:
	pio run -t build e latte_v2
	pio run -t build -e espresso_lite_v2
	cp .pioenvs/latte_v2/firmware.bin binaries/latte.bin
	cp .pioenvs/espresso_lite_v2/firmware.bin binaries/espressolite.bin
	rm data/index.htm
	echo "CURRENT_VERSION=${VERSION}"
	$(SPIFFS) -c data -p 256 -b 8192 -s 524288 binaries/spiffs-512k.bin
spiffs-512:
	rm data/index.htm
	echo "CURRENT_VERSION=${VERSION}"
	$(SPIFFS) -c data -p 256 -b 8192 -s 524288 binaries/spiffs-512k.bin
flash-firmware:
	$(ESPTOOL) -vv -cd nodemcu -cb 460800 -cf .pioenvs/latte_v2/firmware.bin  -cp "/dev/cu.SLAB_USBtoUART" 
flash-spiffs:
	$(ESPTOOL) -vv -cd nodemcu -cb 460800 -ca 0x300000 -cf .pioenvs/latte_v2/spiffs.bin -cp "/dev/cu.SLAB_USBtoUART" 
test:
	./pio_build_test.sh
