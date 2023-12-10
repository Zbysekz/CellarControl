

void ControlWaterPump(){ 
  bool water_detected = digitalRead(PIN_WATER_LEVEL);

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
  /*if(fanControl_autMan){
    if((errorFlags & (1UL << ERROR_SHT20))==0){
      if(SHT_temperature < SHT_dew_point + 1.0){
          fan_onOff = true;
      }else
          fan_onOff = false;
    }else{
      fan_onOff = false;
    }
  }*/
  Serial.println("Fan:"+(String)fan_onOff);
  digitalWrite(PIN_FAN, fan_onOff);
}

void ControlPolyboxAndFermentor(){
  // ------- TEMP CONTROL OF POLYBOX
  if(polybox_autMan){
    if((errorFlags & (1UL << ERROR_TEMP))==0){
      if(temp_polybox > polybox_setpoint + polybox_hysteresis){
          polybox_cool_req = true;
      }
      if(temp_polybox < polybox_setpoint - polybox_hysteresis){
          polybox_cool_req = false;
      }      
    }else{
      polybox_cool_req = false;
    }
  }else polybox_cool_req = false;
  // ------- TEMP CONTROL OF FERMENTOR
  if(fermentor_autMan){
    if((errorFlags & (1UL << ERROR_TEMP))==0){
      if(temp_fermentor > fermentor_setpoint + fermentor_hysteresis){
          fermentor_cool_req = true;
      }
      if(temp_fermentor < fermentor_setpoint - fermentor_hysteresis){
          fermentor_cool_req = false;
      }      
    }else{
      fermentor_cool_req = false;
    }
  }else fermentor_cool_req = false;

  if (fermentor_cool_req || polybox_cool_req){
      chillPump_onOff = true; 
      if(fermentor_cool_req){
        valve_cellar1_onOff = true;
        valve_cellar2_onOff = false; 
      }else{
        valve_cellar1_onOff = false;
        valve_cellar2_onOff = true;
      }
  }else{
    chillPump_onOff = false;
    valve_cellar1_onOff = false;
    valve_cellar2_onOff = false;
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

  digitalWrite(PIN_FREEZER, freezer_onOff);
  digitalWrite(PIN_PUMP, chillPump_onOff && !pump_paused);
  digitalWrite(PIN_BOX_FANS, chillPump_onOff && !pump_paused && !fermentor_cool_req);

  /*
  if(CheckTimer(tmrFreezer,60000L)){//every minute increase / decrease variable
    if(tempControl_onOff){
      if (fridgeCooled < 120) fridgeCooled ++;
    }else{
      if (fridgeCooled > 0)fridgeCooled--;      
    }
  }*/

  digitalWrite(PIN_VALVE_CELLAR1, valve_cellar1_onOff&& !pump_paused);
  digitalWrite(PIN_VALVE_CELLAR2, valve_cellar2_onOff&& !pump_paused);
}

void ControlFermentorHeating(){
  if (!fermentor_autMan && fermentor_heating_onOff &&  ((errorFlags & (1UL << ERROR_TEMP))==0)){
    digitalWrite(PIN_FERMENTER_HEATING, temp_fermentor < fermentor_setpoint);
  }else
  digitalWrite(PIN_FERMENTER_HEATING,false);
}
void ControlGarden(){
  
  //start of watering
  if(clock_valid){
    if((clock_h==watering_morning_hour2 || clock_h==watering_evening_hour2)&&clock_min == 0){
        if(garden2_autMan){
            garden2_onOff = true;
            garden2_watering_tmr = millis();
        }
    }

    if((clock_h==watering_morning_hour3 || clock_h==watering_evening_hour3)&&clock_min == 30){
        if(garden3_autMan){
          Serial.println("SET2 ON!!!");
            garden3_onOff = true;
            garden3_watering_tmr = millis();
        }
    }
    
  }

  if(garden2_autMan && garden2_onOff){
     if((unsigned long)(millis() - garden2_watering_tmr) >= garden2_watering_duration){
       garden2_onOff = false;
       Serial.println("SET2 OFF");
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

  if(CheckTimer(tmrReqClock, 6*60*60000L)){ // 6 hours
      reqClock = true;
    }
  if(CheckTimer(tmrClock, 60000L)){
    if(clock_valid){
      if(++clock_min >= 60){
        clock_min = 0;
        if(++clock_h >=24)
          clock_h = 0;
      }
    }
  }
Serial.print("Garden:");
Serial.print(garden1_onOff);    
Serial.print(garden2_onOff);    
Serial.println(garden3_onOff);    

Serial.println(clock_valid);
Serial.println(clock_h);
Serial.println(clock_min);    

Serial.println(garden2_autMan);
Serial.println(garden2_watering_duration/60000UL);
Serial.println(watering_morning_hour2);

}