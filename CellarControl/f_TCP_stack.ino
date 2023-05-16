void CommWithServer(){

  uint8_t tx_data[30];
  
  uint8_t ptr = 0;
  tx_data[ptr++] = ((uint8_t)(paramsValid)<<0) | ((uint8_t)(errorFlags)<<1) | ((uint8_t)(water_pump_alarm)<<2) | ((uint8_t)(fanControl_autMan)<<3) 
                 | ((uint8_t)(fanControl_onOff)<<4) | ((uint8_t)(tempControl_autMan)<<5) |((uint8_t)(tempPump_onOff)<<6) | ((uint8_t)(0)<<7);


  tx_data[ptr++] = uint8_t((((uint16_t)(SHT_temperature*10))&0xFF00)>>8);
  tx_data[ptr++] = uint8_t(((uint16_t)(SHT_temperature*10))&0xFF);
  tx_data[ptr++] = uint8_t((((uint16_t)(SHT_humidity*10))&0xFF00)>>8);
  tx_data[ptr++] = uint8_t(((uint16_t)(SHT_humidity*10))&0xFF);
  tx_data[ptr++] = uint8_t((((uint16_t)(SHT_dew_point*10))&0xFF00)>>8);
  tx_data[ptr++] = uint8_t(((uint16_t)(SHT_dew_point*10))&0xFF);
 
  tx_data[ptr++] = uint8_t((((uint16_t)(temp_setpoint*10))&0xFF00)>>8);
  tx_data[ptr++] = uint8_t(((uint16_t)(temp_setpoint*10))&0xFF);
  tx_data[ptr++] = uint8_t((((uint16_t)(temp_polybox*10))&0xFF00)>>8);
  tx_data[ptr++] = uint8_t(((uint16_t)(temp_polybox*10))&0xFF);
  tx_data[ptr++] = uint8_t((((uint16_t)(temp_cellar*10))&0xFF00)>>8);
  tx_data[ptr++] = uint8_t(((uint16_t)(temp_cellar*10))&0xFF);

  Send(tx_data, ptr);
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
  
  switch(data[1]){//by ID
    case 0:
      tempControl_autMan = data[2];
    break;
    case 1:
      fanControl_autMan = data[2];
    break;
    case 2:
      tempControl_onOff = data[2];
    break;
    case 3:
      tempPump_onOff = data[2];
    break;
    case 4:
      fanControl_onOff = data[2];
    break;
    case 5:
      temp_setpoint = data[2];
    break; 
    case 199://ending packet
      xServerEndPacket = true;
    break;
    default:
      Serial.println(F("Not defined command!"));
      
  }
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