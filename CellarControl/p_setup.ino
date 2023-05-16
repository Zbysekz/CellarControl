

void setup(){

  Serial.begin(9600);
  //bridgeSerial.begin(9600);

  Serial.println(F("Program start"));

  //pinMode(18, INPUT_PULLUP);//I2C pull-ups
  //pinMode(19, INPUT_PULLUP);

  //setup onewire temperature sensors
  oneWireSensors.begin();

  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_FRIDGE, OUTPUT);
  pinMode(PIN_WATER_PUMP, OUTPUT);
  pinMode(PIN_VALVE_GARDEN1, OUTPUT);
  pinMode(PIN_VALVE_GARDEN2, OUTPUT);
  pinMode(PIN_VALVE_GARDEN3, OUTPUT);
  pinMode(PIN_VALVE_CELLAR1, OUTPUT);
  pinMode(PIN_VALVE_CELLAR2, OUTPUT);
  pinMode(PIN_HEATING, OUTPUT);
  pinMode(PIN_BOX_FANS, OUTPUT);
  pinMode(PIN_RESERVE, OUTPUT);
  pinMode(PIN_WATER_LEVEL, INPUT_PULLUP);


  digitalWrite(PIN_PUMP, false);
  digitalWrite(PIN_FAN, false);
  digitalWrite(PIN_FRIDGE, false);
  digitalWrite(PIN_WATER_PUMP, false);
  digitalWrite(PIN_VALVE_GARDEN1, false);
  digitalWrite(PIN_VALVE_GARDEN2, false);
  digitalWrite(PIN_VALVE_GARDEN3, false);
  digitalWrite(PIN_VALVE_CELLAR1, false);
  digitalWrite(PIN_VALVE_CELLAR2, false);
  digitalWrite(PIN_HEATING, false);
  digitalWrite(PIN_BOX_FANS, false);
  digitalWrite(PIN_RESERVE, false);

  //WIFI

  #define ARR_LEN 8*2
  uint8_t bytes[ARR_LEN];
  
  if(ReadFromEEPROM(bytes, ARR_LEN)){
    uint8_t ptr = 0;
    temp_setpoint = ReadValue(bytes, ptr);
    uint16_t bits = ReadValue(bytes, ptr);

    tempControl_autMan = bits&(1<<0);
    tempControl_onOff = bits&(1<<1);
    fanControl_autMan = bits&(1<<2);
    fanControl_onOff = bits&(1<<3);
    
    paramsValid = true;
  }else{
    Serial.println("Not valid EEPROM data!");
    paramsValid = false;
  }

  fridgeCooled = 0;

  water_pump_out=false;
  

  /*digitalWrite(LED_BUILTIN, false);
  delay(300);
  digitalWrite(LED_BUILTIN, true);
  */
  
  //Wire.begin();
  //sht20.begin();

  OneWireDevicesPrintAddr(); // to find address of dallas temperature sensors

  Serial.println(F("Setup finished"));
}
