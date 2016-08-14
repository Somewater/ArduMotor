# ArduMotor
Purpose of the project: creation of a cheap toy vehicle controlled by Arduino and ESP8266 chip,
which can be driven from a mobile phone.

## Requirements:
1. Arduino chip
2. ESP8266 chip
3. Electrical componens like resistors etc.

## Prepare project:
Add ESP8266 support to Arduino IDE using [guide](https://github.com/esp8266/Arduino#installing-with-boards-manager)

Add [arduino-esp8266fs-plugin](https://github.com/esp8266/arduino-esp8266fs-plugin) for static data uploading
```
$ cp ESP8266/Consts.h.template ESP8266/Consts.h   # edit Consts.h if need
$ npm install inliner
$ generate_final_htm.sh
```

## Build and upload ESP8266 sketch:
1. Plug ESP8266 in programming mode (connect GPIO_0 on GND)
2. Upload static data: `Arduino IDE -> Tools -> ESP8266 Sketch Data Upload`
3. Upload code: `Arduino IDE -> File -> Upload`

## Build and upload Arduino sketch
