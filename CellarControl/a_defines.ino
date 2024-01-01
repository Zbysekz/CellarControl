
DeviceAddress addrTempSensorFermentor = {0x28,0xFF,0x64,0x1E,0x15,0xEA,0x6A,0x9D};
DeviceAddress addrTempSensorPolybox = {0x28,0xFF,0x64,0x1E,0x15,0x13,0x5C,0xD5};
DeviceAddress addrTempSensorCellar = {0x28,0xFF,0x64,0x1F,0x76,0x7F,0xA5,0xE4};

#define EEADDR_START 64

//parameters
float triplePoint1=0.0, boilingPoint1=98.0, triplePoint2=0.0, boilingPoint2=98.0,
triplePoint3=0.0, boilingPoint3=98.0;
//temperatures
float SHT_humidity,SHT_temperature,SHT_dew_point;
float temp_cellar, temp_polybox, temp_fermentor;

//setpoints
float polybox_setpoint;
float polybox_hysteresis = 0.5;
float fermentor_setpoint;
float fermentor_hysteresis = 0.5;

//states
bool polybox_autMan,freezer_onOff,chillPump_onOff;
bool fermentor_autMan, fermentor_heating_onOff;
bool fanControl_autMan,fan_onOff;

bool garden1_autMan, garden2_autMan, garden3_autMan;
bool garden1_onOff, garden2_onOff, garden3_onOff;
bool valve_cellar1_onOff, valve_cellar2_onOff;

bool reqClock,clock_valid;
uint8_t clock_h, clock_min;
unsigned long tmrReqClock, tmrClock;
unsigned long garden2_watering_duration, garden2_watering_tmr;
unsigned long garden3_watering_duration, garden3_watering_tmr;
uint8_t watering_morning_hour2, watering_morning_hour3;
uint8_t watering_evening_hour2, watering_evening_hour3;

// aux vars
bool fermentor_cool_req, polybox_cool_req;

// water pump control
bool water_pump_out, water_detected;
#define WATER_PUMP_MAX_ON_TIME 3*60000UL
#define WATER_PUMP_ON_DELAY_NORMAL 10000UL
#define WATER_PUMP_ON_DELAY_LONG 60*5000UL
#define WATER_PUMP_OFF_DELAY 9000UL
#define WATER_PUMP_ALARM_RESET 10*60000UL

unsigned long WATER_PUMP_ON_DELAY = 0UL;
unsigned long cntWaterPumpActivationsPerHour;
unsigned long pump_last_activations_per_h = 0;
//errors
unsigned long errorFlags;
bool paramsValid = false;

bool water_pump_alarm;
//timers
unsigned long tmrControlLoop,tmrCommWithServer, tmrFreezer, tmrWaterPumpAlarmReset,tmrWaterPumpOnDelay,tmrWaterPumpOnTime,tmrWaterPumpOffDelay,tmrWaterPumpInstensityCnt;
int freezerCooled;
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

#define PUMP_LONG_RUN 30*60000UL
#define PUMP_PAUSE_TIME 30*60000UL

#define PIN_ONE_WIRE_BUS 14
#define PIN_PUMP 9
#define PIN_FAN 15
#define PIN_FREEZER 12
#define PIN_WATER_PUMP 5

#define PIN_VALVE_GARDEN1 6 // beer cooling
#define PIN_VALVE_GARDEN2 10 // hops
#define PIN_VALVE_GARDEN3 11 // tomatos

#define PIN_VALVE_CELLAR1 7  
#define PIN_VALVE_CELLAR2 8

#define PIN_WATER_LEVEL 16
#define PIN_FERMENTER_HEATING 17
#define PIN_BOX_FANS 18
#define PIN_RESERVE 4


#define ERROR_TEMP 0
#define ERROR_SHT20 1

 #define EEPROM_ARR_LEN 22

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(PIN_ONE_WIRE_BUS);

DallasTemperature oneWireSensors(&oneWire);

SoftwareSerial bridgeSerial(2, 3); // RX, TX
