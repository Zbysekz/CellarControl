
DeviceAddress addrTempSensorCellar = {0x28,0xFF,0x64,0x1E,0x15,0xEA,0x6A,0x9D};
DeviceAddress addrTempSensorPolybox = {0x28,0xFF,0x64,0x1E,0x15,0x13,0x5C,0xD5};

uFire_SHT20 sht20;

#define EEADDR_START 0

//parameters
float triplePoint1, boilingPoint1, triplePoint2, boilingPoint2;
//temperatures
float SHT_humidity,SHT_temperature,SHT_dew_point;
float temp_cellar, temp_polybox;

//setpoints
float setpoint;
float hysteresis;

//states
bool tempControl_autMan,tempControl_onOff,tempPump_onOff;
bool fanControl_autMan,fanControl_onOff;


//errors
unsigned long errorFlags;
bool paramsValid = false;
//aux
bool output,heating;
//timers
unsigned long tmrControlLoop,tmrSendDataToServer, tmrFridge;
int fridgeCooled;

#define DATA_SEND_ID 109


#define PIN_ONE_WIRE_BUS 2
#define PIN_PUMP 9
#define PIN_FAN 15
#define PIN_FRIDGE 12
#define PIN_WATER_PUMP 5

#define PIN_VALVE_GARDEN1 6
#define PIN_VALVE_GARDEN2 10
#define PIN_VALVE_GARDEN3 11

#define PIN_VALVE_CELLAR1 7
#define PIN_VALVE_CELLAR2 8

#define PIN_WATER_LEVEL 16
#define PIN_HEATING 17
#define PIN_BOX_FANS 3
#define PIN_RESERVE 4


#define ERROR_TEMP 0
#define ERROR_SHT20 1

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(PIN_ONE_WIRE_BUS);

DallasTemperature oneWireSensors(&oneWire);
