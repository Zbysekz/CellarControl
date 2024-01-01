

void setup(){

  Serial.begin(9600);
  bridgeSerial.begin(9600);

  Serial.println(F("Program start"));

  //pinMode(18, INPUT_PULLUP);//I2C pull-ups
  //pinMode(19, INPUT_PULLUP);

  //setup onewire temperature sensors
  oneWireSensors.begin();

  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_FREEZER, OUTPUT);
  pinMode(PIN_WATER_PUMP, OUTPUT);
  pinMode(PIN_VALVE_GARDEN1, OUTPUT);
  pinMode(PIN_VALVE_GARDEN2, OUTPUT);
  pinMode(PIN_VALVE_GARDEN3, OUTPUT);
  pinMode(PIN_VALVE_CELLAR1, OUTPUT);
  pinMode(PIN_VALVE_CELLAR2, OUTPUT);
  pinMode(PIN_FERMENTER_HEATING, OUTPUT);
  pinMode(PIN_BOX_FANS, OUTPUT);
  pinMode(PIN_RESERVE, OUTPUT);
  pinMode(PIN_WATER_LEVEL, INPUT_PULLUP);
  
  digitalWrite(PIN_PUMP, false);
  digitalWrite(PIN_FAN, false);
  digitalWrite(PIN_FREEZER, false); 
  digitalWrite(PIN_WATER_PUMP, false);
  digitalWrite(PIN_VALVE_GARDEN1, false);
  digitalWrite(PIN_VALVE_GARDEN2, false);
  digitalWrite(PIN_VALVE_GARDEN3, false);
  digitalWrite(PIN_VALVE_CELLAR1, false);
  digitalWrite(PIN_VALVE_CELLAR2, false);
  digitalWrite(PIN_FERMENTER_HEATING, false);
  digitalWrite(PIN_BOX_FANS, false);
  digitalWrite(PIN_RESERVE, false);

  uint8_t bytes[EEPROM_ARR_LEN];
  
  if(ReadFromEEPROM(bytes, EEPROM_ARR_LEN)){
    uint8_t ptr = 0;

    uint16_t bits = ReadValue(bytes, ptr);

    polybox_autMan = bits&(1<<0);
    fanControl_autMan = bits&(1<<1);
    fan_onOff = bits&(1<<2);
    fanControl_autMan = bits&(1<<3);
    freezer_onOff = bits&(1<<4);
    chillPump_onOff = bits&(1<<5);
    fermentor_autMan = bits&(1<<6);
    fermentor_heating_onOff = bits&(1<<7);
    garden1_autMan = bits&(1<<8);
    garden2_autMan = bits&(1<<9);
    garden3_autMan = bits&(1<<10);
    garden1_onOff = bits&(1<<11);
    garden2_onOff = bits&(1<<12);
    garden3_onOff = bits&(1<<13);
    valve_cellar1_onOff = bits&(1<<14);
    valve_cellar2_onOff = bits&(1<<15);

    polybox_setpoint = ReadValuef(bytes, ptr);
    polybox_hysteresis = ReadValuef(bytes, ptr);
    fermentor_setpoint = ReadValuef(bytes, ptr);
    fermentor_hysteresis = ReadValuef(bytes, ptr);
    garden2_watering_duration = ReadValuel(bytes, ptr);
    garden3_watering_duration = ReadValuel(bytes, ptr);
    watering_morning_hour2 = bytes[ptr++];
    watering_morning_hour3 = bytes[ptr++];
    watering_evening_hour2 = bytes[ptr++];
    watering_evening_hour3 = bytes[ptr++];
    Serial.println("Read len:");
    Serial.println(ptr);
    paramsValid = true;
  }else{
    Serial.println("Not valid EEPROM data!");
    paramsValid = false;
  }

  freezerCooled = 0;
  Serial.print("Loaded poly sp:");
  Serial.println(polybox_setpoint);
  water_pump_out=false;
  
  /*digitalWrite(LED_BUILTIN, false);
  delay(300);
  digitalWrite(LED_BUILTIN, true);
  */
  
  //Wire.begin();
  //sht20.begin();

  OneWireDevicesPrintAddr(); // to find address of dallas temperature sensors

  reqClock = true;
  Serial.println(F("Setup finished"));
}

void SaveToEEPROM(){
  
  uint8_t bytes[EEPROM_ARR_LEN];
  uint8_t ptr = 0;

  bytes[ptr++] = ((uint8_t)(polybox_autMan)<<0) | ((uint8_t)(fanControl_autMan)<<1) | ((uint8_t)(fan_onOff)<<2) | ((uint8_t)(fanControl_autMan)<<3) |
                  ((uint8_t)(freezer_onOff)<<4) | ((uint8_t)(chillPump_onOff)<<5) | ((uint8_t)(fermentor_autMan)<<6) | ((uint8_t)(fermentor_heating_onOff)<<7);
  bytes[ptr++] = ((uint8_t)(garden1_autMan)<<0) | ((uint8_t)(garden2_autMan)<<1) | ((uint8_t)(garden3_autMan)<<2) | ((uint8_t)(garden1_onOff)<<3) |
                  ((uint8_t)(garden2_onOff)<<4) | ((uint8_t)(garden3_onOff)<<5) | ((uint8_t)(valve_cellar1_onOff)<<6) | ((uint8_t)(valve_cellar2_onOff)<<7);

  StoreValuef(bytes, polybox_setpoint, ptr);
  StoreValuef(bytes, polybox_hysteresis, ptr);
  StoreValuef(bytes, fermentor_setpoint, ptr);
  StoreValuef(bytes, fermentor_hysteresis, ptr);
  StoreValuel(bytes, garden2_watering_duration, ptr);
  StoreValuel(bytes, garden3_watering_duration, ptr);
  bytes[ptr++] = watering_morning_hour2;
  bytes[ptr++] = watering_morning_hour3;
  bytes[ptr++] = watering_evening_hour2;
  bytes[ptr++] = watering_evening_hour3;
  
  Serial.println("stored len:");
  Serial.println(ptr);
  if(ptr!=EEPROM_ARR_LEN)
    Serial.print("Write param size vs defined MISMATCH!!!!");

  WriteToEEPROM(bytes, EEPROM_ARR_LEN);
}
