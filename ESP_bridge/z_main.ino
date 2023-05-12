
void loop(){

  ArduinoOTA.handle();

  while(Serial.available() > 0) {
    uint8_t rx = Serial.read();
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
        break;
      default:
        if(buffer_ptr == expected_len){
          if(rx == 222){
              SendDataToServer();
          }else{
            receive_state=0;
            buffer_ptr = 0;
          }
        }
      
    }
  }
 
}
