#include "application.h"
#include "SparkFunMAX17043.h"

// Config
static int moisturePin = A0;
static int powerPin = A5;

// Sensor value
static int moisture;

// Battery stats
static double vcell, charge;
static MAX17043 liMon;

// Sampling period (in ms)
const int Ts = 1000;

// Helper routines
void sample();
void updateBatteryStats();

void setup()
{
    // Configure I/O
    pinMode(moisturePin, INPUT);
    pinMode(powerPin, OUTPUT);

    // Declare particle variables for cloud access
    Particle.variable("moisture", &moisture, INT);
    Particle.variable("vcell", &vcell, DOUBLE);
    Particle.variable("charge", &charge, DOUBLE);

    // Initialize the battery monitor
    liMon.begin();
    liMon.quickStart();
}

void loop()
{
    // Sample the moisture reading
    sample();

    // Collect battery stats
    updateBatteryStats();

    // Delay for polling interval - TODO: go into sleep mode
    delay(Ts);
}

void sample()
{
    digitalWrite(powerPin, HIGH);        // Turn on the sensor supply
    delay(200);                          // Give supply time to come up
    moisture = analogRead(moisturePin);  // Read from the ADC
    digitalWrite(powerPin, LOW);         // Turn the sensor supply back off
}

void updateBatteryStats()
{
    vcell = liMon.getVoltage();
    charge = liMon.getSOC();
}
