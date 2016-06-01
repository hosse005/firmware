#include "application.h"
#include "SparkFunMAX17043.h"


// I/O mapping
static int moisturePin = A0;
static int powerPin = A5;

// Sensor value
static int moisture;

// Battery stats
static double vcell, charge;
static MAX17043 liMon;

// Sampling period (in seconds)
static const int Ts = 10;

// Antenna configuration
static String antenna;

// Helper routines
static void sample();
static void updateBatteryStats();
static void publish();


void setup()
{
    // Configure I/O
    pinMode(moisturePin, INPUT);
    pinMode(powerPin, OUTPUT);

    // Declare particle variables for cloud access
    Particle.variable("moisture", &moisture, INT);
    Particle.variable("vcell", &vcell, DOUBLE);
    Particle.variable("charge", &charge, DOUBLE);
    Particle.variable("antenna", &antenna, STRING);

    // Initialize the battery monitor
    liMon.begin();
    liMon.quickStart();

    // Try to use the external antenna first
    if (!WiFi.selectAntenna(ANT_EXTERNAL))
	antenna = "u.FL";
    else
    {
	WiFi.selectAntenna(ANT_INTERNAL);
	antenna = "chip";
    }
}

void loop()
{
    // Sample the moisture reading
    sample();

    // Collect battery stats
    updateBatteryStats();

    // Publish new data event to server
    publish();

    // Sleep for specified sampling interval
    System.sleep(SLEEP_MODE_DEEP, Ts);
}

static void sample()
{
    digitalWrite(powerPin, HIGH);        // Turn on the sensor supply
    delay(200);                          // Give supply time to come up
    moisture = analogRead(moisturePin);  // Read from the ADC
    digitalWrite(powerPin, LOW);         // Turn the sensor supply back off
}

static void updateBatteryStats()
{
    vcell = liMon.getVoltage();
    charge = liMon.getSOC();
}

static void publish()
{
    // Publish our particle variables
    char res[4];

    sprintf(res, "%d", moisture);
    Particle.publish("moisture", res);

    sprintf(res, "%2.1f", charge);
    Particle.publish("charge", res);

    sprintf(res, "%1.2f", vcell);
    Particle.publish("vcell", res);

    // Give hw time to complete upload
    delay(3000);

}
