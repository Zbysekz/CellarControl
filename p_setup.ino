

void ICACHE_FLASH_ATTR setup(){

  Serial.begin(9600);

  Serial.println(F("Program start"));

  pinMode(PIN_ONE_WIRE_BUS, INPUT_PULLUP);

  //setup onewire temperature sensors
  oneWireSensors.begin();

  pinMode(PIN_PUMP, OUTPUT);
  pinMode(PIN_FAN, OUTPUT);
  pinMode(PIN_FRIDGE, OUTPUT);
  digitalWrite(PIN_PUMP, false);
  digitalWrite(PIN_FAN, false);
  digitalWrite(PIN_FRIDGE, false);
  

  //WIFI
  
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);   

  ConnectToWifi();

  server.begin();
  ServerSetup();
  Serial.println("HTTP server started");

  
  ArduinoOTA.setHostname("Cellar A/C unit");
  ArduinoOTA.begin();


  EEPROM.begin(512);

  #define ARR_LEN 8*2
  uint8_t bytes[ARR_LEN];
  if(ReadFromEEPROM(bytes, ARR_LEN)){
    uint8_t ptr = 0;
    setpoint = ReadValue(bytes, ptr);
    hysteresis = ReadValue(bytes, ptr);
    boilingPoint1 = ReadValue(bytes, ptr);
    boilingPoint2 = ReadValue(bytes, ptr);
    triplePoint1 = ReadValue(bytes, ptr);
    triplePoint2 = ReadValue(bytes, ptr);
    fridgeCooled = ReadValue(bytes, ptr);
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
  

  /*digitalWrite(LED_BUILTIN, false);
  delay(300);
  digitalWrite(LED_BUILTIN, true);
  */
  Serial.println(F("Setup finished"));

  OneWireDevicesPrintAddr(); // to find address of dallas temperature sensors

  Wire.begin();
  sht20.begin();

}
