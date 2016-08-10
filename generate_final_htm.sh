#!/bin/bash
# install Inliner as: npm install -g inliner
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
esp_dir=$DIR/ESP8266/data
app_dir=$DIR/OnsenApp/www

pushd "$app_dir" > /dev/null

required_files=(favicon.ico)

rm -rf "$esp_dir"
mkdir "$esp_dir"

for file in "${required_files[@]}"
do
    cp --parents "$file" "$esp_dir"
    if  [[ $file == *.css ]] || [[ $file == *.js ]] || [[ $file == *.htm ]];
    then
        cat "$esp_dir/$file" | gzip --best > "$esp_dir/$file.gz"
        rm "$esp_dir/$file"
    fi
done
inliner -nm index.htm | gzip > "$esp_dir/index.htm.gz"

popd > /dev/null
