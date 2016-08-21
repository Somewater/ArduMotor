#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ln -s $DIR/Arduino/EventDispatcher.h ESP8266/EventDispatcher.h
ln -s $DIR/Arduino/SerialEventDispatcher.h ESP8266/SerialEventDispatcher.h
ln -s $DIR/Arduino/HashMap.h ESP8266/HashMap.h
ln -s $DIR/Arduino/Vector.h ESP8266/Vector.h
