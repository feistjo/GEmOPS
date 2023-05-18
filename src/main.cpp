#include <Arduino.h>

#include "ESP32_FastPWM.h"
#include "display.h"
#include "galvo.h"

Galvo x{19, 22, 9, 0};
Galvo y{13, 14, 9, 0};  // Set pwm and dir
Display out{x, y, 16};  // Set laser pwm

float delay_time = 0.1;
void setup()
{
    Serial.begin(115200);
    std::vector<Display::Point> *circle = new std::vector<Display::Point>{};
    for (uint16_t i = 0; i < 360; i++)
    {
        circle->push_back({100 * cosf(i * M_PI / 180), 100 * cosf(i * M_PI / 180)});
    }
    out.SetImage(circle);
    xTaskCreate([](void *param) { out.DisplayImage(); }, "Display", 1024, NULL, 2, NULL);
    // Serial.println("Ender an int to set galvo: ");
    //  put your setup code here, to run once:
}

void loop()
{
    // position is -100 to 100
    // Make a circle with radius 100
    // static TickType_t xPrevWakeTime = xTaskGetTickCount();
    static uint16_t timer = 0;
    float x_pos = 100 * cosf(timer * M_PI / 180);  // radians
    float y_pos = 100 * cosf(timer * M_PI / 180);
    out.SetPosition(x_pos, y_pos, 100);
    // xTaskDelayUntil(&xPrevWakeTime, delay_time / portTICK_PERIOD_MS);
    // delay(delay_time);
    delayMicroseconds(delay_time * 1000);
    timer = (timer + 1) % 360;
    /* if (timer % 10 == 0)
    {
        Serial.printf("X: %5.2f, Y: %5.2f\n");
    } */

    /* static float pos = -100;
    static float dir = -10;
    Serial.println(pos);
    x.SetPosition(pos);
    delay(delay_time);
    if (pos >= 100.0f || pos <= -100.0f) dir *= -1;
    pos += dir; */
    /* x.SetPosition(255);
    delay(delay_time);
    delay_time *= 0.9;
    if (delay_time < 20) delay_time = 20; */
    /* if (Serial.available())
    {
        String in = Serial.readStringUntil('\n');
        x.SetPosition(std::atoi(in.c_str()));
        Serial.println(std::atoi(in.c_str()));
    } */
}