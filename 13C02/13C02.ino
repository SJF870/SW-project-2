#include <Servo.h>

#define PIN_SERVO 10

#define _DUTY_MIN 500
#define _DUTY_NEU 1500
#define _DUTY_MAX 2500

#define _SERVO_SPEED 0.3

#define INTERVAL 20

Servo myservo;

unsigned long last_sampling_time;

float duty_change_per_interval;
int duty_target;
float duty_curr;

void setup() {
    myservo.attach(PIN_SERVO);

    duty_curr = _DUTY_MIN;
    duty_target = _DUTY_NEU;
    myservo.writeMicroseconds(duty_curr);

    duty_change_per_interval = 
    (_DUTY_MAX - _DUTY_MIN) * (_SERVO_SPEED / 180.0) * (INTERVAL / 1000.0);


}

void loop() {
    if (millis() < (last_sampling_time + INTERVAL))
        return;

    if (duty_target > duty_curr) {
        duty_curr += duty_change_per_interval;
        if (duty_curr > duty_target)
            duty_curr = duty_target;
    } else {
    duty_curr -= duty_change_per_interval;
    if (duty_curr < duty_target)
        duty_curr = duty_target;
    }

    myservo.writeMicroseconds((int)(duty_curr + 0.5f));
    
    last_sampling_time += INTERVAL;
}
