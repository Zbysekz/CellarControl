
AsyncWebServer server(80);

//wifi
bool connectedToWifi = false;

// the IP address for the shield:
IPAddress ip(192, 168, 0, 33);

IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress ipServer(192, 168, 0, 3);  

WiFiClient wifiClient;

#define BUFFER_SIZE 50
long unsigned int tmrSendDataToServer;
uint8_t read_buffer[BUFFER_SIZE];
uint8_t rx, expected_len, buffer_ptr, receive_state;