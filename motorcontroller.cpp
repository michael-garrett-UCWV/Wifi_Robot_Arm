/*
    MotorController Class
*/

#include "motorcontroller.h"

// Constructor
MotorController::MotorController(float speed, float frametime) {
    this->speed = speed;
    this->frametime = frametime;
}

// Servo Input Array
// angle, max angle, control pin, min, max, hertz
void MotorController::setup_servos(std::list<std::list<int>> servo_list)
{
    for (std::list<int> servo_params : servo_list) 
    {
        // Get parameters
        int angle = servo_params.front(); 
        servo_params.pop_front();

        int max_angle = servo_params.front(); 
        servo_params.pop_front();

        int control_pin = servo_params.front(); 
        servo_params.pop_front();

        int min = servo_params.front(); 
        servo_params.pop_front();

        int max = servo_params.front(); 
        servo_params.pop_front();

        int hertz = servo_params.front(); 
        servo_params.pop_front();

        // Create servo
        ServoMotor servo = ServoMotor(angle,            // Angle
                                        max_angle,      // Max Angle
                                        control_pin,    // Control Pin
                                        min,            // Min
                                        max,            // Max
                                        hertz);         // Hertz

        // Add servo to controller's list
        this->servos.push_back(servo);
    }
}

// Stepper Input Array
// angle, direction pin, step pin, resolution
void MotorController::setup_stepper(std::list<int> stepper_params)
{
    // Get parameters
    int angle = stepper_params.front(); 
    stepper_params.pop_front();

    int dirPin = stepper_params.front();
    stepper_params.pop_front();

    int stepPin = stepper_params.front(); 
    stepper_params.pop_front();

    int resolution = stepper_params.front(); 
    stepper_params.pop_front();

    // Create stepper
    this->stepper = Stepper(angle, dirPin, stepPin, resolution);
}

/*
    Move motors smoothly
    Angles list should start with stepper motor and work up to end of arm
*/
void MotorController::move_motors_smooth(std::list<int> angles)
{
    int largestAngleDelta = 0;

    // Set motor angles
    // Stepper motor
    int stepperAngle = angles.front();
    angles.pop_front();
    stepper.set_angle(stepperAngle);
    largestAngleDelta = max(largestAngleDelta, stepperAngle);

    // Servo motors
    for (int i = 0; i < this->servos.size(); i++) 
    {
        int servoAngle = angles.front();
        angles.pop_front();
        this->servos[i].Motor::set_angle(servoAngle);
        largestAngleDelta = max(largestAngleDelta, servoAngle);

    }

    // Calculate largest angle
    float duration = largestAngleDelta / this->speed;
    float steps = duration / this->frametime;
    Serial.println("Duration: " + String(duration));
    Serial.println("Steps: " + String(steps));
    Serial.println("Speed: " + String(this->speed));
    Serial.println("Frame time: " + String(this->frametime));


    if (steps == 0.0f) { 
        steps = 1.0f; 
    }

    float progress = 0.0f;
    float startTime = millis();

    while (progress < 1.0f) {
        float currentTime = millis();

        progress += 1.0f / steps;
        progress = min(1.0f, progress);

        Serial.println("Progress: " + String(progress));

        // Move Stepper
        stepper.move(progress);

        // Move servos
        for (int i = 0; i < this->servos.size(); i++) 
        {
            this->servos[i].move(progress);
        }

        // Wait for step to be completed
        float waitDelay = max(0.0f, (this->frametime * 1000) - (millis() - currentTime));
        Serial.println("Delay: " + String(waitDelay));
        delay(waitDelay);
    }
}

/*
    Move motors fast
    Angles list should start with stepper motor and work up to end of arm
*/
void MotorController::move_motors(std::list<int> angles)
{
    // Set motor angles
    // Stepper motor
    int stepperAngle = angles.front();
    angles.pop_front();
    Serial.println("Stepper angle: " + String(stepperAngle));
    stepper.set_angle(stepperAngle);

    // Servo motors
    for (int i = 0; i < this->servos.size(); i++) 
    {
        int servoAngle = angles.front();
        angles.pop_front();
        Serial.println("Servo angle: " + String(servoAngle));
        this->servos[i].Motor::set_angle(servoAngle);
    }

    // Move Stepper
    stepper.move(1);

    // Move servos
    for (int i = 0; i < this->servos.size(); i++) 
    {
        this->servos[i].move(1);
    }
}