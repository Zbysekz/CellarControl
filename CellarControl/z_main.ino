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
      
      if(!ReadTemperatures()) //read all temp probes
        errorFlags |= 1UL << ERROR_TEMP;
      else
        errorFlags &= ~(1UL << ERROR_TEMP); // clear that error
      
      
      if((errorFlags & (1UL << ERROR_TEMP))==0){
      ;//Serial.println(temp_cellar);          
          //    digitalWrite(PIN_FERMENTER_HEATING, temp_cellar < 0);
      }else{
        digitalWrite(PIN_FERMENTER_HEATING,false);
      }
      ControlWaterPump();
      ControlFan();
      ControlPolyboxAndFermentor();
      ControlGarden();
      Serial.print("polysetpoint:");
      Serial.println(polybox_setpoint);
      Serial.println(polybox_autMan);
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

