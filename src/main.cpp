#include <Arduino.h>

#include "galvo.h"

Galvo x{19, 22, -255, 255, 8, 0};

void setup()
{
    Serial.begin(115200);
    Serial.println("Ender an int to set galvo: ");
    // put your setup code here, to run once:
}

void loop()
{
    String in = Serial.readStringUntil('\n');
    x.SetPosition(std::atoi(in.c_str()));
    // put your main code here, to run repeatedly:
}