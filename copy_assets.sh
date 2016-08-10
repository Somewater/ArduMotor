#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
esp_dir=$DIR/ESP8266/data
app_dir=$DIR/OnsenApp/www

pushd "$app_dir" > /dev/null

required_files=(a/font-awesome.min.css
                a/fontawesome-webfont.woff2
                a/onsen-css-comp.min.css
                a/onsenui.min.css
                a/onsenui.min.js
                index.htm
                favicon.ico)

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

popd > /dev/null