#ifndef UTILS_H
#define UTILS_H

#include "Arduino.h"
#include <MD5Builder.h>

String md5(String str);
String md5(uint8_t * data, uint16_t len);

#endif