#include "application.h"

// Config
static int moisturePin = A0;
static int powerPin = A5;

// Sensor value
static int moisture;

void setup()
{
    // Configure I/O
    pinMode(moisturePin, INPUT);
    pinMode(powerPin, OUTPUT);

    // Declare a moisture particle variable for cloud access
    Particle.variable("moisture", &moisture, INT);

    // Set sensor power on all the time
    digitalWrite(powerPin, HIGH);
}

void loop()
{
    // Sample the moisture reading - every second
    //digitalWrite(powerPin, HIGH);
    //delay(200);
    moisture = analogRead(moisturePin);
    //digitalWrite(powerPin, LOW);
    delay(1000);
}
