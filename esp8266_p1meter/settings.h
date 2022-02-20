// **********************************
// * Settings                       *
// **********************************

// Update treshold in milliseconds, messages will only be sent on this interval
#define UPDATE_INTERVAL 5000  // 5 seconds

// * Baud rate for both hardware and software 
#define BAUD_RATE 115200

// * Wifi timeout in milliseconds
#define WIFI_TIMEOUT 30000

// * MQTT topic
#define MQTT_PUBLISH_TOPIC "p1meter/data"

long LAST_UPDATE_SENT = 0;

#define THINGNAME "p1meter"

// * Set to store the data values read
float CONSUMPTION_LOW_TARIF;
float CONSUMPTION_HIGH_TARIF;

float RETURNDELIVERY_LOW_TARIF;
float RETURNDELIVERY_HIGH_TARIF;

float ACTUAL_CONSUMPTION;
float ACTUAL_RETURNDELIVERY;
float GAS_METER_M3;

float L1_INSTANT_POWER_USAGE;
float L2_INSTANT_POWER_USAGE;
float L3_INSTANT_POWER_USAGE;
float L1_INSTANT_POWER_CURRENT;
float L2_INSTANT_POWER_CURRENT;
float L3_INSTANT_POWER_CURRENT;
float L1_VOLTAGE;
float L2_VOLTAGE;
float L3_VOLTAGE;

// Set to store data counters read
float ACTUAL_TARIF;
float SHORT_POWER_OUTAGES;
float LONG_POWER_OUTAGES;
float SHORT_POWER_DROPS;
float SHORT_POWER_PEAKS;
