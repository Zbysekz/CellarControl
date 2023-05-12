

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
    /*if (request->hasParam("newSetpoint",true)) {
      inputMessage = request->getParam("newSetpoint",true)->value();
      setpoint = inputMessage.toFloat();
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
    }*/}
    // -----------------------------------------------------------------

    // Display current state    
    char result[2000];
    strcpy(result,index_html);

    //strcat(result,("<form action=\"/\" method=\"post\"><input type=\"submit\" value=\""+String(fanControl_onOff?"state: ON":"state: OFF")+"\" name=\"fanControl_onOff\"></form><br><br>").c_str());
    
    
    //strcat(result,("<p>Cellar temp: " + formatVal(temp_cellar) + " \xB0""C</p>").c_str());
    //strcat(result,("<p>Free heap: " + String(ESP.getFreeHeap()) +"</p>").c_str());

    strcat(result, "</body></html>");
    request->send_P(200, "text/html", result);
    
  });
  //server.onNotFound(notFound);
  server.begin();
}
