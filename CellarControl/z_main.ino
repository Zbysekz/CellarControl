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
      Serial.println("gu");
      if(!ReadTemperatures()) //read all temp probes
        errorFlags |= 1UL << ERROR_TEMP;
      else
        errorFlags &= ~(1UL << ERROR_TEMP); // clear that error
      
      Serial.println("go");

      ControlWaterPump();
      ControlFan();
      ControlPolybox();
        
    }

  }

  if(CheckTimer(tmrCommWithServer, 30000L)){
    CommWithServer();
  }
 
}

