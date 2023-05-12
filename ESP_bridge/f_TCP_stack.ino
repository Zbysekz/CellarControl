
void ConnectToWifi(){
  //zkusíme se připojit k serveru pokud ještě nejsme
    Serial.println("Connecting to wifi");
    WiFi.begin(ssid, password);
    Serial.println("Waiting for connection result..");

    unsigned long wifiConnectStart = millis();
    
    while (WiFi.status() != WL_CONNECTED) {
        // Check to see if
        if (WiFi.status() == WL_CONNECT_FAILED) {
          Serial.println("Failed to connect to WiFi");
        }
    
        delay(500);
        // Only try for 5 seconds.
        if (millis() - wifiConnectStart > 15000) {
          Serial.println("Failed to connect to WiFi");
        }
    }
    Serial.println(F("Connected!"));   
}
void SendDataToServer(){
//vytvorime spojeni, posleme data a precteme jestli nejaka neprisla, pak zavreme spojeni
    /*if (!wifiClient.connect(ipServer, 23)) {
          Serial.println("Connection to server failed! IP:"+ipServer.toString());
        } else {

        uint8_t bits = uint8_t(digitalRead(PIN_FAN));
          
        uint8_t sbuf[] = {DATA_SEND_ID,int(temp_cellar*100)/256, int(temp_cellar*100)%256,int(temp_polybox*100)/256, int(temp_polybox*100)%256,
        int(SHT_temperature*100)/256, int(SHT_temperature*100)%256,int(SHT_humidity*100)/256, int(SHT_humidity*100)%256,
        int(SHT_dew_point*100)/256, int(SHT_dew_point*100)%256, bits};

        int cnt = Send(sbuf,12);
        if(cnt<=0){
          Serial.println("Write failed!");
        }

        delay(50);
        while(wifiClient.available()){
          uint8_t rcv = wifiClient.read();
          //Receive(rcv);
          //Serial.write(rcv);
        }
            
        }
        wifiClient.stop();
        Serial.println("Data sent to server!");*/
}


int Send(uint8_t d[],uint8_t d_len){
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

  return wifiClient.write(data,6+d_len);
}
