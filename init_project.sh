#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ln -s $DIR/Arduino/EventDispatcher.h $DIR/ESP8266/EventDispatcher.h
ln -s $DIR/Arduino/SerialEventDispatcher.h $DIR/ESP8266/SerialEventDispatcher.h
ln -s $DIR/Arduino/HashMap.h $DIR/ESP8266/HashMap.h
ln -s $DIR/Arduino/Vector.h $DIR/ESP8266/Vector.h
