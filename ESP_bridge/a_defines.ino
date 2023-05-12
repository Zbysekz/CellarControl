
AsyncWebServer server(80);

//wifi
bool connectedToWifi = false;

// the IP address for the shield:
IPAddress ip(192, 168, 0, 33);

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ipServer(192, 168, 0, 3);  

WiFiClient wifiClient;

#define DATA_SEND_ID 109

long unsigned int tmrSendDataToServer;
