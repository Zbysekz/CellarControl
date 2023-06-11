
DeviceAddress addrTempSensorCellar = {0x28,0xFF,0x64,0x1E,0x15,0xEA,0x6A,0x9D};
DeviceAddress addrTempSensorPolybox = {0x28,0xFF,0x64,0x1E,0x15,0x13,0x5C,0xD5};

uFire_SHT20 sht20;

#define EEADDR_START 0

//parameters
float triplePoint1=0.0, boilingPoint1=98.0, triplePoint2=0.0, boilingPoint2=98.0;
//temperatures
float SHT_humidity,SHT_temperature,SHT_dew_point;
float temp_cellar, temp_polybox;

//setpoints
float temp_setpoint;
float temp_hysteresis = 0.5;

//states
bool tempControl_autMan,tempControl_onOff,tempPump_onOff;
bool fanControl_autMan,fanControl_onOff;
bool water_pump_alarm;

// water pump control
bool water_pump_out;
#define WATER_PUMP_MAX_ON_TIME 3*60000UL
#define WATER_PUMP_ON_DELAY 10000UL
#define WATER_PUMP_OFF_DELAY 8000UL
#define WATER_PUMP_ALARM_RESET 10*60000UL


//errors
unsigned long errorFlags;
bool paramsValid = false;

//timers
unsigned long tmrControlLoop,tmrCommWithServer, tmrFridge, tmrWaterPumpAlarmReset,tmrWaterPumpOnDelay,tmrWaterPumpOnTime,tmrWaterPumpOffDelay;
int fridgeCooled;
unsigned long tmrPumpPause, tmrPumpLongRun;
bool pump_paused;

//receive
#define RXBUFFSIZE 20
#define RXQUEUESIZE 3

uint8_t errorCnt_dataCorrupt, errorCnt_CRCmismatch, errorCnt_BufferFull;
uint8_t rxBuffer[RXQUEUESIZE][RXBUFFSIZE];//for first item if >0 command is inside, 0 after it is proccessed
bool rxBufferMsgReady[RXQUEUESIZE];
uint8_t rxLen,crcH,crcL,readState,rxPtr,rxBufPtr=0;
bool xServerEndPacket;

int gi;//for for loops in switch-case

#define DATA_SEND_ID 109

#define PUMP_LONG_RUN 3*60000UL
#define PUMP_PAUSE_TIME 30*60000UL

#define PIN_ONE_WIRE_BUS 14
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
#define PIN_FERMENTER_HEATING 17
#define PIN_BOX_FANS 18
#define PIN_RESERVE 4


#define ERROR_TEMP 0
#define ERROR_SHT20 1

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(PIN_ONE_WIRE_BUS);

DallasTemperature oneWireSensors(&oneWire);

SoftwareSerial bridgeSerial(2, 3); // RX, TX
