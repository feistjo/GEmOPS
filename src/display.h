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
        current_point_idx_ = -1;
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
                Point current_point_ = points_->at(current_point_idx_);
                uint16_t next_point_idx = current_point_idx_ + 1 < points_->size() ? current_point_idx_ + 1 : 0;
                Point next_point_ = points_->at(next_point_idx);
                float dx = next_point_.x - current_point_.x;
                float dy = next_point_.y - current_point_.y;
                float dist = dx + dy;
                if ((dx >= 0 ? current_point_.x + (dx * kVelocity / dist) >= next_point_.x
                             : current_point_.x + (dx * kVelocity / dist) <= next_point_.x)
                    || current_point_.y + (dy * kVelocity / dist) >= next_point_.y)
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
    uint8_t pwm_channel{0};
    uint8_t laser_pwm_pin_;
    ESP32_FAST_PWM* laser_pwm_;

    const float kVelocity{1};

    Point current_point_{0, 0};
    int16_t current_point_idx_ = -1;
    std::vector<Point>* points_ = nullptr;
};