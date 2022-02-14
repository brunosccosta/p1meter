// **********************************
// * Settings                       *
// **********************************

// Update treshold in milliseconds, messages will only be sent on this interval
#define REMOTE_UPDATE_INTERVAL 60000  // 1 minute
#define LOCAL_UPDATE_INTERVAL 5000  // 5 seconds

// * Baud rate for both hardware and software 
#define BAUD_RATE 115200

// * Wifi timeout in milliseconds
#define WIFI_TIMEOUT 30000

// * Heartbeat in milliseconds
#define MQTT_HEARTBEAT 10000

// * AWS root topic
#define AWS_IOT_HEARTBEAT_TOPIC "p1meter/alive"
#define AWS_IOT_PUBLISH_TOPIC "p1meter/data"

long LAST_LOCAL_UPDATE_SENT = 0;
long LAST_REMOTE_UPDATE_SENT = 0;
long LAST_HEARTBEAT_SENT = 0;

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
