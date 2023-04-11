#include <Arduino.h>

#include <cstdint>

class Galvo
{
public:
    Galvo(uint8_t pwm, uint8_t dir, int min_pwm, int max_pwm, uint8_t n_bits, bool invert_dir)
        : pwm_{pwm}, dir_{dir}, min_pwm_{min_pwm}, max_pwm_{max_pwm}, n_bits_{n_bits}, invert_dir_{invert_dir}
    {
        pinMode(pwm_, OUTPUT);
        pinMode(dir_, OUTPUT);
    }

    void SetPosition(uint8_t position)
    {
        int desired_pwm = map(position, 0, 1 << position, min_pwm_, max_pwm_);
        digitalWrite(dir_, invert_dir_ ? desired_pwm > 0 : desired_pwm < 0);
        analogWrite(pwm_, abs(desired_pwm));
    }

private:
    uint8_t pwm_;
    uint8_t dir_;
    int min_pwm_;
    int max_pwm_;
    uint8_t n_bits_;
    bool invert_dir_;
};