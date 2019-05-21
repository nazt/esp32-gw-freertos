#!/bin/bash

pushd cmmcAdmin 
VERSION=`node -pe "require('./package.json').version"`
echo "VVVV: ${VERSION}"
NEXT_VERSION=`node -pe "require('semver').inc('$VERSION', 'patch')"`
echo $NEXT_VERSION
node -e "\
	var j = require('./package.json');\
j.version = '${NEXT_VERSION}';\
	var s = JSON.stringify(j, null, 4) + '\n';\
	require('fs').writeFileSync('./package.json', s);"

./spiffs.sh
rm -v dist/static/img/*.svg
rm -v dist/static/js/app*.js
rm -v dist/static/js/vendor*.js
rm -v dist/static/css/*.css
rm -v dist/static/fonts/*.ttf
rm -v dist/static/fonts/*.eot
rm -v dist/static/fonts/*.woff
rm -v dist/static/fonts/*.woff2 

popd 
rsync --delete -av cmmcAdmin/dist/ data --progress
rsync -av wifi.json data/
rsync -av mymqtt.json data/
rsync -av sensors.json data/
rsync -av espnow.json data/

# pio run -t uploadfs 
