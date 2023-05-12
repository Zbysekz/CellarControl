

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta http-equiv='refresh' content='30'> 
  </head><body>
  <h1>Cellar A/C unit</h1>
  <form action="/" method="post">
    setpoint: <input type="text" name="newSetpoint">
    <input type="submit" value="set">
  </form><br>
  <form action="/" method="post">
    hysteresis: <input type="text" name="newHysteresis">
    <input type="submit" value="set">
  </form><br>
  <form action="/" method="post">
    boiling point: <input type="text" name="newBoilingPoint1">
    <input type="submit" value="set">
  </form><br>
  <form action="/" method="post">
    triple point: <input type="text" name="newTriplePoint1">
    <input type="submit" value="set">
  </form><br>
  <form action="/" method="post">
    boiling point polybox: <input type="text" name="newBoilingPoint2">
    <input type="submit" value="set">
  </form><br>
  <form action="/" method="post">
    triple point polybox: <input type="text" name="newTriplePoint2">
    <input type="submit" value="set">
  </form><br>)rawliteral";
  
 
/*
           */ 
/*void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}*/



void ServerSetup(){
  // Send web page with input fields to client
  server.on("/", HTTP_POST | HTTP_GET, [](AsyncWebServerRequest *request){

    if(request->method() == HTTP_POST){
    // check for commands -------------------------------------------
    bool anyChange = true;
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam("newSetpoint",true)) {
      inputMessage = request->getParam("newSetpoint",true)->value();
      setpoint = inputMessage.toFloat();
    }else if (request->hasParam("newHysteresis",true)) {
      inputMessage = request->getParam("newHysteresis",true)->value();
      hysteresis = inputMessage.toFloat();
    }else if (request->hasParam("newBoilingPoint1",true)) {
      inputMessage = request->getParam("newBoilingPoint1",true)->value();
      boilingPoint1 = inputMessage.toFloat();
    }else if (request->hasParam("newTriplePoint1",true)) {
      inputMessage = request->getParam("newTriplePoint1",true)->value();
      triplePoint1 = inputMessage.toFloat();
    }else if (request->hasParam("newBoilingPoint2",true)) {
      inputMessage = request->getParam("newBoilingPoint2",true)->value();
      boilingPoint2 = inputMessage.toFloat();
    }else if (request->hasParam("newTriplePoint2",true)) {
      inputMessage = request->getParam("newTriplePoint2",true)->value();
      triplePoint2 = inputMessage.toFloat();
    }else if (request->hasParam("tempControl_autMan",true)) {
      tempControl_autMan = !tempControl_autMan;
    }else if (request->hasParam("fanControl_autMan",true)) {
      fanControl_autMan = !fanControl_autMan;
    }else if (request->hasParam("tempControl_onOff",true)) {
      tempControl_onOff = !tempControl_onOff;
    }else if (request->hasParam("fanControl_onOff",true)) {
      fanControl_onOff = !fanControl_onOff;
    }else if (request->hasParam("tempPump_onOff",true)) {
      tempPump_onOff = !tempPump_onOff;
    }
    else {
      anyChange = false;
    }

    if(anyChange){
      #define ARR_LEN 8*2
      uint8_t bytes[ARR_LEN];
      uint8_t ptr = 0;
     
      StoreValue(bytes, setpoint, ptr);
      StoreValue(bytes, hysteresis, ptr);
      StoreValue(bytes, boilingPoint1, ptr);
      StoreValue(bytes, boilingPoint2, ptr);
      StoreValue(bytes, triplePoint1, ptr);
      StoreValue(bytes, triplePoint2, ptr);
      StoreValue(bytes, fridgeCooled, ptr);

      
      uint16_t bits = (tempControl_autMan<<0) + (tempControl_onOff<<1) + (fanControl_autMan<<2) + (fanControl_onOff<<3);
      StoreValue(bytes, bits, ptr);
      
      
  
      WriteToEEPROM(bytes, ARR_LEN);
    }}
    // -----------------------------------------------------------------

    // Display current state    
    char result[2000];
    strcpy(result,index_html);

    strcat(result,("<form action=\"/\" method=\"post\"><input type=\"submit\" value=\""+String(tempControl_autMan?"TEMP automat":"TEMP manual")+"\" name=\"tempControl_autMan\"></form>").c_str());
    strcat(result,("<form action=\"/\" method=\"post\"><input type=\"submit\" value=\""+String(tempControl_onOff?"state: ON":"state: OFF")+"\" name=\"tempControl_onOff\"></form><br>").c_str());
    strcat(result,("<form action=\"/\" method=\"post\"><input type=\"submit\" value=\""+String(tempPump_onOff?"PUMP ON":"PUMO OFF")+"\" name=\"tempPump_onOff\"></form>").c_str());
    strcat(result,("<form action=\"/\" method=\"post\"><input type=\"submit\" value=\""+String(fanControl_autMan?"FAN automat":"FAN manual")+"\" name=\"fanControl_autMan\"></form>").c_str());
    strcat(result,("<form action=\"/\" method=\"post\"><input type=\"submit\" value=\""+String(fanControl_onOff?"state: ON":"state: OFF")+"\" name=\"fanControl_onOff\"></form><br><br>").c_str());
    
    
    //strcat(result,("<p>Cellar temp: " + formatVal(temp_cellar) + " \xB0""C</p>").c_str());
    strcat(result,("<p>Polybox temp: " + formatVal(temp_polybox) + " \xB0""C</p>").c_str());
    //strcat(result,("<p>Room humid: " + formatVal(SHT_humidity) + " %</p>").c_str());
    //strcat(result,("<p>Room temp: " + formatVal(SHT_temperature) + " \xB0""C</p>").c_str());
    strcat(result,("<p>Errors: " + formatVal(errorFlags) + "</p>").c_str());
    
    strcat(result,("<p>Setpoint: " + formatVal(setpoint) + " \xB0""C</p>").c_str());
    strcat(result,("<p>Hyst: " + formatVal(hysteresis) + " \xB0""C</p>").c_str());
    //strcat(result,("<p>Cal.cellar-triple point: " + formatVal(triplePoint1) + " \xB0""C</p>").c_str());
    //strcat(result,("<p>Cal.cellar-boiling point: " + formatVal(boilingPoint1) + " \xB0""C</p>").c_str());
    //strcat(result,("<p>Cal.polybox-triple point: " + formatVal(triplePoint2) + " \xB0""C</p>").c_str());
    //strcat(result,("<p>Cal.polybox-boiling point: " + formatVal(boilingPoint2) + " \xB0""C</p>").c_str());
    //strcat(result,("<p>Free heap: " + String(ESP.getFreeHeap()) +"</p>").c_str());

    strcat(result, "</body></html>");
    request->send_P(200, "text/html", result);
    
  });
  //server.onNotFound(notFound);
  server.begin();
}
