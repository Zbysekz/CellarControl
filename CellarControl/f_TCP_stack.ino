void CommWithServer(){

  uint8_t tx_data[50];
  
  uint8_t ptr = 0;
  tx_data[ptr++] = DATA_SEND_ID;
  tx_data[ptr++] = ((uint8_t)(paramsValid)<<0) | ((uint8_t)(errorFlags)<<1) | ((uint8_t)(water_pump_alarm)<<2) | ((uint8_t)(fanControl_autMan)<<3) 
                 | ((uint8_t)(fan_onOff)<<4) | ((uint8_t)(polybox_autMan)<<5) |((uint8_t)(chillPump_onOff)<<6) | ((uint8_t)(freezer_onOff)<<7);
  tx_data[ptr++] = ((uint8_t)(fermentor_autMan)<<0) | ((uint8_t)(fermentor_heating_onOff)<<1) | ((uint8_t)(garden1_autMan)<<2) | ((uint8_t)(garden1_onOff)<<3) 
                 | ((uint8_t)(garden2_autMan)<<4) | ((uint8_t)(garden2_onOff)<<5) |((uint8_t)(garden3_autMan)<<6) | ((uint8_t)(garden3_onOff)<<7);
  tx_data[ptr++] = ((uint8_t)(valve_cellar1_onOff)<<0) | ((uint8_t)(valve_cellar2_onOff)<<1) | ((uint8_t)(0)<<2) | ((uint8_t)(0)<<3) 
                 | ((uint8_t)(0)<<4) | ((uint8_t)(0)<<5) |((uint8_t)(0)<<6) | ((uint8_t)(0)<<7);

  Store_TX_float(tx_data, SHT_temperature, ptr);
  Store_TX_float(tx_data, SHT_humidity, ptr);
  Store_TX_float(tx_data, SHT_dew_point, ptr);

  Store_TX_float(tx_data, polybox_setpoint, ptr);
  Store_TX_float(tx_data, polybox_hysteresis, ptr);
  Store_TX_float(tx_data, fermentor_setpoint, ptr);
  Store_TX_float(tx_data, fermentor_hysteresis, ptr);

  Store_TX_float(tx_data, temp_polybox, ptr);
  Store_TX_float(tx_data, temp_cellar, ptr);
  Store_TX_float(tx_data, temp_fermentor, ptr);

  Send(tx_data, ptr);
}

void Store_TX_float(uint8_t *arr, float val, uint8_t& ptr){
  arr[ptr++] = uint8_t((((uint16_t)(val*10))&0xFF00)>>8);
  arr[ptr++] = uint8_t(((uint16_t)(val*10))&0xFF);
}

void Send(uint8_t d[],uint8_t d_len){
  uint8_t data[6+d_len];
 
  data[0]=111;//start byte
  data[1]=222;//start byte

  data[2]=d_len;//delka

  for(int i=0;i<d_len;i++)
    data[3+i]=d[i];//data
  
  uint16_t crc = CRC16(data+2, d_len+1);

  data[3+d_len]=crc/256;
  data[4+d_len]=crc%256;
  data[5+d_len]=222;//end byte

  bridgeSerial.write(data,6+d_len);
}


void ProcessReceivedData(uint8_t data[]){
  int res=0;//aux temp
  int len = data[0];
  //uint16_t auxVal = 0;
  Serial.println("Received data from server!");
  Serial.println(data[0]);
  Serial.println(data[1]);
  Serial.println(data[2]);
  
  switch(data[1]){//by ID
    case 0:
      polybox_autMan = data[2];
    break;
    case 1:
      fanControl_autMan = data[2];
    break;
    case 2:
      freezer_onOff = data[2];
    break;
    case 3:
      chillPump_onOff = data[2];
    break;
    case 4:
      fan_onOff = data[2];
    break;
    case 5:
      polybox_setpoint = data[2];
      polybox_hysteresis = data[3]/10.0;
    break; 
    case 6:
      fermentor_autMan = data[2];
    break;
    case 7:
      fermentor_setpoint = data[2];
      fermentor_hysteresis = data[3]/10.0;
    break;
    case 20:
      garden1_autMan = data[2];
    break;
    case 21:
      garden2_autMan = data[2];
    break;
    case 22:
      garden3_autMan = data[2];
    break;
    case 23:
      garden1_onOff = data[2];
    break; 
    case 24:
      garden2_onOff = data[2];
    break; 
    case 25:
      garden3_onOff = data[2];
    break; 
    case 26:
      valve_cellar1_onOff = data[2];
    break;
    case 27:
      valve_cellar2_onOff = data[2];
    break; 
    case 28:
      garden2_watering_duration = data[2] * 60000UL; // in minutes
      garden3_watering_duration = data[3] * 60000UL;
      watering_evening_hour2 = data[4];
      watering_evening_hour3 = data[5];
    case 240: // time synchronization
      time_ntp_hour = data[2];
      new_ntp_arrived = true;
    break;
    case 199://ending packet
      xServerEndPacket = true;
    break;
    default:
      Serial.println(F("Not defined command!"));
      
  }
  if (data[1]!=199 && data[1]!=240)
    SaveToEEPROM();
}

void ProcessReceivedData(){
  for(int i=0;i<RXQUEUESIZE;i++)
    if(rxBufferMsgReady[i]==true){
      ProcessReceivedData(rxBuffer[i]);
      rxBufferMsgReady[i]=false;//mark as processed
    }
}

void Receive(uint8_t rcv){
    switch(readState){
    case 0:
      if(rcv==111){readState=1;}//start token
      break;
    case 1:
      if(rcv==222)readState=2;else { //second start token
        if(errorCnt_dataCorrupt<255)errorCnt_dataCorrupt++; 
        readState=0;
        Serial.println(F("RxErr0"));
      }
      break;
    case 2:
      rxLen = rcv;//length
      if(rxLen>RXBUFFSIZE){//should not be so long
        readState=0;
        if(errorCnt_dataCorrupt<255)errorCnt_dataCorrupt++;
        Serial.println(F("RxErr1")); 
      }else{ 
        readState=3;
        rxPtr=0;
        //choose empty stack
        rxBufPtr=99;
        for(gi=0;gi<RXQUEUESIZE;gi++){
          if(rxBufferMsgReady[gi]==false)
            rxBufPtr=gi;
        }
        if(rxBufPtr==99){
          if(errorCnt_BufferFull<255)errorCnt_BufferFull++;
           Serial.println(F("RxErr2"));
          readState=0;
        }else{
          rxBuffer[rxBufPtr][rxPtr++]=rxLen;//at the start is length
        }
      }
      break;
    case 3://receiving data itself
      rxBuffer[rxBufPtr][rxPtr++] = rcv;
      
      if(rxPtr>=RXBUFFSIZE || rxPtr>=rxLen+1){
        readState=4;
      }
      break;
    case 4:
      crcH=rcv;//high crc
      readState=5;
      break;
    case 5:
      crcL=rcv;//low crc

      if(CRC16(rxBuffer[rxBufPtr], rxLen+1) == crcL+(uint16_t)crcH*256){//crc check
        readState=6;
      }else {
        readState=0;//CRC not match
        if(errorCnt_CRCmismatch<255)errorCnt_CRCmismatch++;
        Serial.println(F("RxErr3"));
      }
      break;
    case 6:
      if(rcv==222){//end token
        rxBufferMsgReady[rxBufPtr]=true;//mark this packet as complete
      }else{
        Serial.println(F("RxErr4"));
        if(errorCnt_dataCorrupt<255)errorCnt_dataCorrupt++; 
      }
      readState=0;
      break;
    }
}