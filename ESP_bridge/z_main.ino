

//after turning on OUTPUT, active time starts to count, if expire, lagTime start to count and output is inhibited in that moment
//after lagTime expiration, normal process repeats

void loop(){

  ArduinoOTA.handle();

    if(CheckTimer(tmrSendDataToServer, 30000L)){
      SendDataToServer();
    }

 
}
