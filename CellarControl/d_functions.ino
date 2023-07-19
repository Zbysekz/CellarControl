//function for creating periodic block code
//returns true if period has passed, resetting the timer
bool CheckTimer(unsigned long &timer, const unsigned long period){
   if((unsigned long)(millis() - timer) >= period){//protected from rollover by type conversion
    timer = millis();
    return true;
   }else{
    return false;
   }
}
void ResetTimer(unsigned long &timer){
  timer = millis();
}
// function to print a device address
void PrintAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}


String formatVal(float val, int minWidth=5, int precision=1);
   
String formatVal(float val, int minWidth, int precision){
   char buff[10];
   dtostrf(val, minWidth, precision, buff);
   String str(buff);
   return str;
}

void StoreValuef(uint8_t *arr, float val, uint8_t &ptr){
   arr[ptr++] = (int)(val*10)/256;
   arr[ptr++] = (int)(val*10)%256;
}

float ReadValuef(uint8_t *arr, uint8_t &ptr){
  float result = (arr[ptr]*256 + arr[ptr+1])/10.0;
  arr+=2;
  return result;
}
uint32_t ReadValuel(uint8_t *arr, uint8_t &ptr){
  uint32_t result = (arr[ptr]*16777216UL + arr[ptr+1]*65536UL + arr[ptr+2]*256 + arr[ptr+3]);
  arr+=4;
  return result;
}
uint16_t ReadValue(uint8_t *arr, uint8_t &ptr){
  uint16_t result = (arr[ptr]*256 + arr[ptr+1]);
  arr+=2;
  return result;
}
void StoreValue(uint8_t *arr, uint16_t val, uint8_t &ptr){
   arr[ptr++] = val/256;
   arr[ptr++] = val%256;
}
void StoreValuel(uint8_t *arr, uint32_t val, uint8_t &ptr){
   arr[ptr++] = (uint8_t)((val&0xFF000000)>>24);
   arr[ptr++] = (uint8_t)((val&0xFF0000)>>16);
   arr[ptr++] = (uint8_t)((val&0xFF00)>>8);
   arr[ptr++] = (uint8_t)(val%256);
}

//calculation of CRC16, corresponds to CRC-16/XMODEM on https://crccalc.com/﻿
uint16_t CRC16(uint8_t* bytes, uint8_t length)
{
    const uint16_t generator = 0x1021; // divisor is 16bit 
    uint16_t crc = 0; // CRC value is 16bit 

    for (int b=0;b<length;b++)
    {
        crc ^= (uint16_t)(bytes[b] << 8); // move byte into MSB of 16bit CRC

        for (int i = 0; i < 8; i++)
        {
            if ((crc & 0x8000) != 0) // test for MSb = bit 15
            {
                crc = (uint16_t)((crc << 1) ^ generator);
            }
            else
            {
                crc <<= 1;
            }
        }
    }
    return crc;
}

void OneWireDevicesPrintAddr(){
  int oneWireDeviceCnt = oneWireSensors.getDeviceCount();

  Serial.print("One wire devices found: ");
  Serial.print(oneWireDeviceCnt, DEC);
  DeviceAddress tempDeviceAddress;
  for(int i=0;i<oneWireDeviceCnt; i++) {
    // Search the wire for address
    if(oneWireSensors.getAddress(tempDeviceAddress, i)) {
      Serial.print("Found device with address: ");
      PrintAddress(tempDeviceAddress);
      Serial.println();
    } else {
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
    }
  }
}


bool ReadTemperature(DeviceAddress address, uint32_t errorCode, float &value){
  float tempC = oneWireSensors.getTempC(address);
  if(tempC==-127){
    errorFlags |= 1UL << errorCode;
    value = 0.0;
    return false;
  }else{
    errorFlags &= ~(1UL << errorCode);
    value = tempC;
    return true;
  }
}

bool ReadTemperatures(){
  bool res = true;
  int cnt = oneWireSensors.getDeviceCount();
  //check if all sensors are connected
  if(cnt != 2){
    oneWireSensors.begin();//try to scan
    if(cnt != oneWireSensors.getDeviceCount())//if not success, return false
      res = false;
  }

  oneWireSensors.requestTemperatures();

  res = ReadTemperature(addrTempSensorCellar, 1, temp_cellar);  
  if(res)
    temp_cellar = CalibrateTemp(temp_cellar, triplePoint1, boilingPoint1);

  res = res && ReadTemperature(addrTempSensorPolybox, 1, temp_polybox);  
  if(res)
    temp_polybox = CalibrateTemp(temp_polybox, triplePoint2, boilingPoint2);
  
  res = res && ReadTemperature(addrTempSensorFermentor, 1, temp_fermentor);  
  if(res)
    temp_polybox = CalibrateTemp(temp_fermentor, triplePoint2, boilingPoint2);
    
  return res;
}
float CalibrateTemp(float raw, float triplePointBath, float boilingPoint){
  //boiling point 99.2°C for 254 m.n.m
  const float ref_boilingPoint = 99.2;
  const float ref_triplePointBath = 0.0;


  return ((raw - triplePointBath)*(ref_boilingPoint-ref_triplePointBath))/(boilingPoint-triplePointBath) + ref_triplePointBath;
}

void WriteToEEPROM(uint8_t bytes[], int len){
  int addr = EEADDR_START;
  for(int i=0;i<len;i++){
     EEPROM.write(addr++, bytes[i]);
  }
  int crc = CRC16(bytes, len);
  EEPROM.write(addr++, crc/256);
  EEPROM.write(addr++, crc%256); 
}

bool ReadFromEEPROM(uint8_t* bytes, int len){
  int addr = EEADDR_START;
 
  for(int i=0;i<len;i++){
     bytes[i] = EEPROM.read(addr++);
  }
  int crcReal = CRC16(bytes, len);
  int crcStored = EEPROM.read(addr)*256 + EEPROM.read(addr+1);
  addr += 2;
  return (crcReal == crcStored);
}
