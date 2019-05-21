#!/bin/bash

npm run build

cp -v dist/index.html dist/index.htm
rm -v dist/static/fonts/*.ttf
rm -v dist/static/img/*.svg
rm -v dist/static/js/app*.js
rm -v dist/static/js/vendor*.js
rm -v dist/static/css/*.css
