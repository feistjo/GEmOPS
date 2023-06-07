#include <Arduino.h>

#include "ESP32_FastPWM.h"
#include "display.h"
#include "galvo.h"
#include "web_interface.h"

Galvo x{19, 22, 9, 0, 0};
Galvo y{13, 14, 9, 0, 1};  // Set pwm and dir
Display out{x, y, 16};     // Set laser pwm
/* Galvo x{0, 1, 9, 0, 0};
Galvo y{2, 3, 9, 0, 1};
Display out{x, y, 4};  // Set laser pwm */

std::vector<Display::Point> points{};

WebInterface web_interface{"GEmOPS", "pleasebuy", out};

float delay_time = 0.1;
void setup()
{
    Serial.begin(115200);
    web_interface.Initialize();
    /* std::vector<Display::Point> *circle = new std::vector<Display::Point>{}; */
    for (uint16_t i = 0; i < 360; i++)
    {
        points.push_back({50 + (50 * cosf(i * M_PI / 180)), 50 + (50 * cosf(i * M_PI / 180))});
    }
    /* circle->push_back({0, -100});
    circle->push_back({0, 100});
    circle->push_back({100, 0}); */
    out.SetImage(&points);
    xTaskCreatePinnedToCore(
        [](void *param) { out.DisplayImage(); }, "Display", 4096, NULL, 2, NULL, ARDUINO_RUNNING_CORE);
    // vTaskDelete(NULL);
    //   Serial.println("Ender an int to set galvo: ");
    //    put your setup code here, to run once:
}

void loop()
{
    // position is -100 to 100
    // Make a circle with radius 100
    // static TickType_t xPrevWakeTime = xTaskGetTickCount();
    static uint16_t timer = 0;
    float x_pos = 100 * cosf(timer * M_PI / 180);  // radians
    float y_pos = 100 * cosf(timer * M_PI / 180);
    // out.SetPosition(x_pos, y_pos, 100);
    //  xTaskDelayUntil(&xPrevWakeTime, delay_time / portTICK_PERIOD_MS);
    //  delay(delay_time);
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