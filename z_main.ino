

//after turning on OUTPUT, active time starts to count, if expire, lagTime start to count and output is inhibited in that moment
//after lagTime expiration, normal process repeats

void loop(){

  ArduinoOTA.handle();

  //digitalWrite(PIN_RELE, true);
  
  if(paramsValid){
    if(CheckTimer(tmrControlLoop, 5000L)){

      
      if (!sht20.connected())
        errorFlags |= 1UL << ERROR_SHT20;
      else
        errorFlags &= ~(1UL << ERROR_SHT20); // clear that error

      
      SHT_humidity = sht20.humidity();
      SHT_temperature = sht20.temperature();
      SHT_dew_point = sht20.dew_point();
      
      
      if(!ReadTemperatures())
        errorFlags |= 1UL << ERROR_TEMP;
      else
        errorFlags &= ~(1UL << ERROR_TEMP); // clear that error
  

      // ------- FAN CONTROL
      if(fanControl_autMan){
        if((errorFlags & (1UL << ERROR_SHT20))==0){
          if(SHT_temperature < SHT_dew_point + 1.0){
             fanControl_onOff = true;
          }else
             fanControl_onOff = false;
        }else{
          fanControl_onOff = false;
        }
       
      }
      digitalWrite(PIN_FAN, fanControl_onOff);
    
      // ------- TEMP CONTROL
      if(tempControl_autMan){
        if((errorFlags & (1UL << ERROR_TEMP))==0){
          if(temp_polybox > setpoint + hysteresis){
             tempPump_onOff = true;
          }
          if(temp_polybox < setpoint - hysteresis){
             tempPump_onOff = false;
          }      
        }else{
          tempPump_onOff = false;
        }
        
      }
      
      digitalWrite(PIN_FRIDGE, tempControl_onOff);
      if(tempPump_onOff){
        digitalWrite(PIN_PUMP, true);
      }else{
        digitalWrite(PIN_PUMP, false);
      }
    }/*
    if(CheckTimer(tmrFridge,60000L)){//every minute increase / decrease variable
      if(tempControl_onOff){
        if (fridgeCooled < 120) fridgeCooled ++;
      }else{
        if (fridgeCooled > 0)fridgeCooled--;      
      }
    }*/
    if(CheckTimer(tmrSendDataToServer, 30000L)){
      SendDataToServer();
    }
  }else{
    Serial.println(F("Parameters not valid!"));
  }

 
}
