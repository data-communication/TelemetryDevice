#include "rgb_led.h"
#include <Arduino.h>

void rgb_init(){
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
}

void rgb_red(){
    digitalWrite(LED_RED, 0);
    digitalWrite(LED_GREEN, 1);
    digitalWrite(LED_BLUE, 1);
}

void rgb_green(){
    digitalWrite(LED_RED, 1);
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_BLUE, 1);
}

void rgb_blue(){
    digitalWrite(LED_RED, 1);
    digitalWrite(LED_GREEN, 1);
    digitalWrite(LED_BLUE, 0);
}

void rgb_yellow(){
    digitalWrite(LED_RED, 0);
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_BLUE, 1);
}

void rgb_magenta(){
    digitalWrite(LED_RED, 0);
    digitalWrite(LED_GREEN, 1);
    digitalWrite(LED_BLUE, 0);
}

void rgb_cyan(){
    digitalWrite(LED_RED, 1);
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_BLUE, 0);
}

void rgb_white(){
    digitalWrite(LED_RED, 0);
    digitalWrite(LED_GREEN, 0);
    digitalWrite(LED_BLUE, 0);
}

void rgb_off(){
    digitalWrite(LED_RED, 1);
    digitalWrite(LED_GREEN, 1);
    digitalWrite(LED_BLUE, 1);
}