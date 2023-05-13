#pragma once

#include <Arduino.h>

#include "ESP32_FastPWM.h"

class Galvo
{
public:
    Galvo(uint8_t pwm, uint8_t dir, uint8_t n_bits, bool invert_dir)
        : pwm_pin_{pwm},
          pwm_{new ESP32_FAST_PWM(pwm_pin_, pwm_frequency, 0, pwm_channel, pwm_resolution)},
          dir_{dir},
          n_bits_{n_bits},
          invert_dir_{invert_dir}
    {
        // pinMode(pwm_, OUTPUT);
        pinMode(dir_, OUTPUT);
    }

    void SetPosition(float position)
    {
        // float desired_pwm = float_map(position, -1 * (1 << (n_bits_ - 1)), (1 << (n_bits_ - 1)) - 1, 0.0f, 100.0f);
        digitalWrite(dir_, invert_dir_ ? position > 0 : position < 0);
        // Serial.println(desired_pwm > 0);
        // Serial.println(abs(position));
        //  analogWrite(pwm_, abs(desired_pwm));
        pwm_->setPWM(pwm_pin_, pwm_frequency, abs(position));
    }

private:
    float pwm_resolution{8};
    float pwm_frequency{200000};
    uint8_t pwm_channel{0};
    uint8_t pwm_pin_;
    ESP32_FAST_PWM* pwm_;
    uint8_t dir_;
    uint8_t n_bits_;
    bool invert_dir_;

    float float_map(float in, float min_in, float max_in, float min_out, float max_out)
    {
        return (((in - min_in) / (max_in - min_in)) * (max_out / min_out)) + min_out;
    }
};