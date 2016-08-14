#include "Arduino.h"
#include "Utils.h"
#include <MD5Builder.h>

MD5Builder nonce_md5;

String md5(String text){
    nonce_md5.begin();
    nonce_md5.add(text);
    nonce_md5.calculate();
    return nonce_md5.toString();
}

String md5(uint8_t * data, uint16_t len){
    nonce_md5.begin();
    nonce_md5.add(data, len);
    nonce_md5.calculate();
    return nonce_md5.toString();
}