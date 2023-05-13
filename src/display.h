#pragma once

#include "ESP32_FastPWM.h"
#include "galvo.h"

class Display
{
public:
    Display(Galvo& x, Galvo& y, uint8_t laser_pwm_pin)
        : x_{x},
          y_{y},
          laser_pwm_pin_{laser_pwm_pin},
          laser_pwm_{new ESP32_FAST_PWM(laser_pwm_pin_, pwm_frequency, 0, pwm_channel, pwm_resolution)}
    {
    }

    void SetPosition(float x, float y, float intensity)
    {
        x_.SetPosition(x);
        y_.SetPosition(y);
        laser_pwm_->setPWM(laser_pwm_pin_, pwm_frequency, intensity);
    }

private:
    Galvo& x_;
    Galvo& y_;
    float pwm_resolution{8};
    float pwm_frequency{200000};
    uint8_t pwm_channel{0};
    uint8_t laser_pwm_pin_;
    ESP32_FAST_PWM* laser_pwm_;
};