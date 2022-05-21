/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

#define servo 2
#define trig 7
#define echo 6

Servo myservo; // create servo object to control a servo
// twelve servo objects can be created on most boards

int pos = 0; // variable to store the servo position

void setup()
{
    myservo.attach(servo); // attaches the servo on pin 9 to the servo object
    pinMode(trig, OUTPUT);
    pinMode(echo, INPUT);
    Serial.begin(9600);
}

int distance()
{
    int echo_d = 0;
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    unsigned long s = micros();
    unsigned long e = micros();
    while (micros() - s < 10000)
    {
        if (digitalRead(echo) && !echo_d)
        {
            s = micros();
            echo_d = 1;
        }

        if (!digitalRead(echo) && echo_d)
        {
            e = micros();
            break;
        }
    }

    float distance = float(e - s) * 0.0343 / 2.0;
    if (distance > 180)
        distance = 180.0;
    // Serial.println(distance);
    return distance;
}

void loop()
{
    myservo.write(distance());
    // Serial.println(distance());
    // distance();
    delay(15);
}
