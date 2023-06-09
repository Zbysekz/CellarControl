

void ControlWaterPump(){ 
  bool water_detected = !digitalRead(PIN_WATER_LEVEL);

  if (!water_pump_alarm){
    ResetTimer(tmrWaterPumpAlarmReset);
    if (!water_pump_out){
      if(water_detected){
        if(CheckTimer(tmrWaterPumpOnDelay, WATER_PUMP_ON_DELAY)){
          water_pump_out=true;
        }
      }else{
        ResetTimer(tmrWaterPumpOnDelay);
      }
    }else{
        if(!water_detected){
          ResetTimer(tmrWaterPumpOnTime);
          if(CheckTimer(tmrWaterPumpOffDelay, WATER_PUMP_OFF_DELAY)){
            water_pump_out=false;
          }
        }else{
          ResetTimer(tmrWaterPumpOffDelay);
          if(CheckTimer(tmrWaterPumpOnTime, WATER_PUMP_MAX_ON_TIME)){
            water_pump_out=false;
            water_pump_alarm=true;
          }
        }
        
    }
  }else{
    water_pump_out = false;
    ResetTimer(tmrWaterPumpOnDelay);
    ResetTimer(tmrWaterPumpOnTime);
    ResetTimer(tmrWaterPumpOffDelay);

    if(CheckTimer(tmrWaterPumpAlarmReset, WATER_PUMP_ALARM_RESET)){ //after longer time, try again
      water_pump_alarm=false;
    }
  }

  digitalWrite(PIN_WATER_PUMP, water_pump_out);
}

void ControlFan(){
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
}

void ControlPolybox(){
  tempControl_autMan = true;
  tempControl_onOff = true;
  temp_setpoint = 2;
  temp_hysteresis = 0.5;
  // ------- TEMP CONTROL
  if(tempControl_autMan){
    if((errorFlags & (1UL << ERROR_TEMP))==0){
      if(temp_polybox > temp_setpoint + temp_hysteresis){
          tempPump_onOff = true;
      }
      if(temp_polybox < temp_setpoint - temp_hysteresis){
          tempPump_onOff = false;
      }      
    }else{
      tempPump_onOff = false;
    }
  }
  
 
  if (!tempPump_onOff){
    ResetTimer(tmrPumpLongRun);
    pump_paused = false; 
  }
  if(!pump_paused && CheckTimer(tmrPumpLongRun, PUMP_LONG_RUN)){
    ResetTimer(tmrPumpLongRun);
    ResetTimer(tmrPumpPause);
    pump_paused = true;
  }
  if (pump_paused && CheckTimer(tmrPumpPause, PUMP_PAUSE_TIME)){
    pump_paused = false;
    ResetTimer(tmrPumpLongRun);
  }

  Serial.print("paused:");
  Serial.println(pump_paused);
 
  Serial.print("tmr:");
  Serial.println(tmrPumpLongRun);

  Serial.print("temp_poly:");
  Serial.println(temp_polybox);

  digitalWrite(PIN_FRIDGE, tempControl_onOff);
  digitalWrite(PIN_PUMP, tempPump_onOff && !pump_paused);
  digitalWrite(PIN_BOX_FANS, tempPump_onOff && !pump_paused);

  /*
  if(CheckTimer(tmrFridge,60000L)){//every minute increase / decrease variable
    if(tempControl_onOff){
      if (fridgeCooled < 120) fridgeCooled ++;
    }else{
      if (fridgeCooled > 0)fridgeCooled--;      
    }
  }*/
}