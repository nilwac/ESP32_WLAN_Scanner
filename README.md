ESP32 WiFi Scanner


Scan available acces points and save them into an appwrite database

Add following file inside scr folder:

project_folder/src/config.h 


fill out these lines:


const char* wifi_ssid = "Your WiFi SSID";

const char* wifi_password = "Your WiFi Password";

String serverIP = "123.456.789.123";

String databaseID = "abcdfghijklmnopqrstuvwxyz";

String collectionID = "0123456790";

String connectionProtokol = "http:// or https://";

String httpHeaderType = "application/json";

String httpHeaderProject = "Your Project ID";

String httpHeaderKey = "Your API Key";

int submit_delay = 10;

int loop_delay = 1000;

int wifi_disconnect_delay = 100;

