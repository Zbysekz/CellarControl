void loop(){

  if(paramsValid){

    if(CheckTimer(tmrControlLoop, 5000L)){
      /*if (!sht20.connected()){
        errorFlags |= 1UL << ERROR_SHT20;
        Serial.println("Not connected!!");
      }
      else{
        errorFlags &= ~(1UL << ERROR_SHT20); // clear that error
        Serial.println("Connected!!");
        SHT_humidity = sht20.humidity();
        SHT_temperature = sht20.temperature();
        SHT_dew_point = sht20.dew_point();
        
        Serial.println("Temp:");
        Serial.println(SHT_temperature);
      }*/

      Serial.print("water_detected:");
      Serial.println(water_detected);

      Serial.print("cnt_per_h:");
      Serial.println(cntWaterPumpActivationsPerHour);
      Serial.print("alarm:");
      Serial.println(water_pump_alarm);
      Serial.println(tmrWaterPumpOnDelay);      
      
      if(!ReadTemperatures()) //read all temp probes
        errorFlags |= 1UL << ERROR_TEMP;
      else
        errorFlags &= ~(1UL << ERROR_TEMP); // clear that error
      
      
      ControlWaterPump();
      ControlFan();
      ControlPolyboxAndFermentor();
      ControlFermentorHeating();
      ControlGarden();
    }

  }

  if(CheckTimer(tmrCommWithServer, 30000L)){
    CommWithServer();
  }

  ProcessReceivedData();
  while(bridgeSerial.available() > 0){
    Serial.print(".");
    uint8_t x = bridgeSerial.read();
    //Serial.println(x);
    Receive(x);
  }
 
}

