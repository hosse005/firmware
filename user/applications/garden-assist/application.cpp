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
static void sync();

// Cloud access routine
static int cloudCtl(String command);
static bool cloudSync = true;

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

    // Declare particle function for cloud invocation
    Particle.function("cloudHook", cloudCtl);

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

    // TODO - publish new data event to server

    // Synchronize with server and then sleep
    sync();
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

static int cloudCtl(String command)
{
    int result = -1;

    /* Commands:
       sync - server to invoke after fetching sensor data
    */
    if (command == "sync")
    {
	cloudSync = true;
	result = 0;
    }

    return result;
}

static void sync()
{
    cloudSync = false;

    // Spin until we receive synchronization from the server - then sleep
    while(!cloudSync) { delay(1000); }
    System.sleep(SLEEP_MODE_DEEP, Ts);
}
