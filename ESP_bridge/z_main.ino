
void loop(){

  ArduinoOTA.handle();

  timeClient.update();

  Serial.println(timeClient.getFormattedTime());
  if(timeClient.getMinutes() == 0){
    if (!minutes_latch){
      minutes_latch = true;
      uint8_t data[2];
      data[0] = 240; // ID of time sync
      data[1] = timeClient.getHours();
      SendToArduino(data, 2);
    }
  }else{
    minutes_latch = false;
  }

  while(bridgeSerial.available() > 0) {
    Serial.println("RECEIVING something");
    uint8_t rx = bridgeSerial.read();
    Serial.println(rx);
    Serial.print("state:");
    Serial.println(receive_state);
    read_buffer[buffer_ptr++] = rx;
    if (buffer_ptr>=BUFFER_SIZE){
      receive_state = 0;
      buffer_ptr = 0;
    }
    switch(receive_state){
      case 0:
        if(rx==111)
          receive_state++;
        else{
          receive_state=0;
          buffer_ptr = 0;
        }
      break;
      case 1:
      if(rx==222)
          receive_state++;
      else{
          receive_state=0;
          buffer_ptr = 0;
        }
      break;
      case 2:
        expected_len = rx+5; //two start flags + crc two bytes + end flag
        receive_state++;
        break;
      default:
        if(buffer_ptr-1 == expected_len){
          if(rx == 222){
              Serial.println("completed, going to send it to server");
              SendDataToServer();
              receive_state=0;
              buffer_ptr = 0;
          }else{
            receive_state=0;
            buffer_ptr = 0;
          }
        } else if (buffer_ptr-1 > expected_len){ // TODO add timeout, when nothing received for certain time
            receive_state=0;
            buffer_ptr = 0;          
        }
      
    }
  }
 
}
