#pragma once

#include <vector>

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

    struct Point
    {
        float x;
        float y;
    };

    // Takes ownership over the image, frees old image
    void SetImage(std::vector<Point>* new_image)
    {
        if (points_)
        {
            free(points_);
        }
        points_ = new_image;
        current_point_idx_ = 0;
        pos_in_interpolation_ = 0;
    }

    void SetPosition(float x, float y, float intensity)
    {
        x_.SetPosition(x);
        y_.SetPosition(y);
        laser_pwm_->setPWM(laser_pwm_pin_, pwm_frequency, intensity);
    }

    void SetPosition(Point point, float intensity) { SetPosition(point.x, point.y, intensity); }

    void DisplayImage()
    {
        for (;;)
        {
            if (points_)
            {
                /* while (1)
                {
                    Serial.println(current_point_idx_);
                    delay(500);
                } */
                Point current_point_ = points_->at(current_point_idx_);
                uint16_t next_point_idx = current_point_idx_ + 1 < points_->size() ? current_point_idx_ + 1 : 0;
                Point next_point_ = points_->at(next_point_idx);
                float dx = next_point_.x - current_point_.x;
                float dy = next_point_.y - current_point_.y;
                float dist = abs(dx + dy);
                // Serial.println(current_point_.x + (pos_in_interpolation_ * dx * kVelocity / dist));
                if ((dx >= 0 ? current_point_.x + (pos_in_interpolation_ * dx * kVelocity / dist) >= next_point_.x
                             : current_point_.x + (pos_in_interpolation_ * dx * kVelocity / dist) <= next_point_.x)
                    && (dy >= 0 ? current_point_.y + (pos_in_interpolation_ * dy * kVelocity / dist) >= next_point_.y
                                : current_point_.y + (pos_in_interpolation_ * dy * kVelocity / dist) <= next_point_.y))
                {
                    Serial.printf("dx: %4.2f, dy: %4.2f\n", dx, dy);
                    current_point_idx_ = next_point_idx;
                    pos_in_interpolation_ = 0;
                }
                else
                {
                    pos_in_interpolation_++;
                }
                // Serial.println(current_point_.y + (pos_in_interpolation_ * kVelocity * dy / dist));
                SetPosition({current_point_.x + (pos_in_interpolation_ * kVelocity * dx / dist),
                             current_point_.y + (pos_in_interpolation_ * kVelocity * dy / dist)},
                            100);
                delayMicroseconds(kDelayTime);
            }
            else
            {
                SetPosition({0, 0}, 0);
            }
        }
    }

private:
    Galvo& x_;
    Galvo& y_;
    float pwm_resolution{8};
    float pwm_frequency{200000};
    uint8_t pwm_channel{2};
    uint8_t laser_pwm_pin_;
    ESP32_FAST_PWM* laser_pwm_;

    const float kVelocity{0.1};
    const uint32_t kDelayTime{10};
    uint32_t pos_in_interpolation_ = 0;

    Point current_point_{0, 0};
    uint16_t current_point_idx_ = 0;
    std::vector<Point>* points_ = nullptr;
};