
DeviceAddress addrTempSensorCellar = {0x28,0xFF,0x64,0x1E,0x15,0xEA,0x6A,0x9D};
DeviceAddress addrTempSensorPolybox = {0x28,0xFF,0x64,0x1E,0x15,0x13,0x5C,0xD5};

AsyncWebServer server(80);

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

//wifi
bool connectedToWifi = false;

const char* ssid = "ZZ_wifi2";
const char* password = "*****";

// the IP address for the shield:
IPAddress ip(192, 168, 0, 33);

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ipServer(192, 168, 0, 3);  

WiFiClient wifiClient;

#define DATA_SEND_ID 109


#define PIN_ONE_WIRE_BUS 12
#define PIN_PUMP 16
#define PIN_FAN 13
#define PIN_FRIDGE 14

#define ERROR_TEMP 0
#define ERROR_SHT20 1

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(PIN_ONE_WIRE_BUS);

DallasTemperature oneWireSensors(&oneWire);
