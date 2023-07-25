

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
          fan_onOff = true;
      }else
          fan_onOff = false;
    }else{
      fan_onOff = false;
    }
  }

  digitalWrite(PIN_FAN, fan_onOff);
}

void ControlPolyboxAndFermentor(){
  // ------- TEMP CONTROL
  if(polybox_autMan){
    if((errorFlags & (1UL << ERROR_TEMP))==0){
      if(temp_polybox > polybox_setpoint + polybox_hysteresis){
          chillPump_req = true;
      }
      if(temp_polybox < polybox_setpoint - polybox_hysteresis){
          chillPump_req = false;
      }      
    }else{
      chillPump_req = false;
    }
  }
  
 
  if (!chillPump_req){
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

  chillPump_onOff = chillPump_req && !pump_paused;


  if(fermentor_autMan){

  }

  digitalWrite(PIN_FREEZER, freezer_onOff);
  digitalWrite(PIN_PUMP, chillPump_onOff);
  digitalWrite(PIN_BOX_FANS, chillPump_onOff);

  /*
  if(CheckTimer(tmrFreefer,60000L)){//every minute increase / decrease variable
    if(tempControl_onOff){
      if (fridgeCooled < 120) fridgeCooled ++;
    }else{
      if (fridgeCooled > 0)fridgeCooled--;      
    }
  }*/

  digitalWrite(PIN_VALVE_CELLAR1, valve_cellar1_onOff);
  digitalWrite(PIN_VALVE_CELLAR2, valve_cellar2_onOff);
}

void ControlGarden(){
  
  //start of watering
  if(new_ntp_arrived){
    new_ntp_arrived = false;
    if(time_ntp_hour==watering_morning_hour2 || time_ntp_hour==watering_evening_hour2){
        if(garden2_autMan){
            garden2_onOff = true;
            garden2_watering_tmr = millis();
        }
    }

    if(time_ntp_hour==watering_morning_hour3 || time_ntp_hour==watering_evening_hour3){
        if(garden3_autMan){
            garden3_onOff = true;
            garden3_watering_tmr = millis();
        }
    }
    
  }

  if(garden2_autMan && garden2_onOff){
     if((unsigned long)(millis() - garden2_watering_tmr) >= garden2_watering_duration){
       garden2_onOff = false;
     }
  }
  if(garden3_autMan && garden3_onOff){
     if((unsigned long)(millis() - garden3_watering_tmr) >= garden3_watering_duration){
       garden3_onOff = false;
     }
  }
  

  digitalWrite(PIN_VALVE_GARDEN1, garden1_onOff);
  digitalWrite(PIN_VALVE_GARDEN2, garden2_onOff);
  digitalWrite(PIN_VALVE_GARDEN3, garden3_onOff);
}