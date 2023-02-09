//#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "wifiFix.h"
#include "config.h"

// WiFi Connection Credentials
const char* ssid = wifi_ssid;
const char* password = wifi_password;

//Your Domain name with URL path or IP address with path "/v1/databases/{databaseId}/collections/{collectionId}/documents"
String serverName = connectionProtokol + serverIP + "/v1/databases/" + databaseID + "/collections/" + collectionID + "/documents";

// WiFi Scan Data
DynamicJsonDocument wifiData(1024);
DynamicJsonDocument wifiData_compl(1024);
String json;

// Fix for WiFi Client -> Buffur overrun
WiFiClient* client = new WiFiClientFixed();
HTTPClient http;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
}

void loop() {
    // Set WiFi to station mode and disconnect from an AP if it was previously connected.
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(wifi_disconnect_delay);

    // Start Scan of Wifi
    int n = WiFi.scanNetworks();
    Serial.println("Scan done");
    
    // Connect to Wifi and send data
    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi");
    while(WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("connected");
    
    // Transmitting data
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        //Check WiFi connection status
        if(WiFi.status()== WL_CONNECTED){
            for (int i = 0; i < n; ++i) {
                // Your Domain name with URL path or IP address with path
                http.begin(*client, serverName);
                // Specify content-type header
                http.addHeader("Content-Type", httpHeaderType);
                http.addHeader("X-Appwrite-Project", httpHeaderProject);
                http.addHeader("X-Appwrite-Key", httpHeaderKey);
                // insert into json SSID, RSSI and Channel for each network found
                wifiData["SSID"] = WiFi.SSID(i).c_str();
                wifiData["RSSI"] = String(WiFi.RSSI(i));
                wifiData["Channel"] = String(WiFi.channel(i));
                // insert into json Encryption for each network found
                switch (WiFi.encryptionType(i)) {
                    case WIFI_AUTH_OPEN:
                        wifiData["Encryption"] = "open";
                        break;
                    case WIFI_AUTH_WEP:
                        wifiData["Encryption"] = "WEP";
                        break;
                    case WIFI_AUTH_WPA_PSK:
                        wifiData["Encryption"] = "WPA";
                        break;
                    case WIFI_AUTH_WPA2_PSK:
                        wifiData["Encryption"] = "WPA2";
                        break;
                    case WIFI_AUTH_WPA_WPA2_PSK:
                        wifiData["Encryption"] = "WPA+WPA2";
                        break;
                    case WIFI_AUTH_WPA2_ENTERPRISE:
                        wifiData["Encryption"] = "WPA2-EAP";
                        break;
                    case WIFI_AUTH_WPA3_PSK:
                        wifiData["Encryption"] = "WPA3";
                        break;
                    case WIFI_AUTH_WPA2_WPA3_PSK:
                        wifiData["Encryption"] = "WPA2+WPA3";
                        break;
                    case WIFI_AUTH_WAPI_PSK:
                        wifiData["Encryption"] = "WAPI";
                        break;
                    default:
                        wifiData["Encryption"] = "unknown";
                    };
                
                // Prepare submission json
                wifiData_compl["documentId"] = "unique()";
                // add network json data into submission json
                wifiData_compl["data"] = wifiData;
                // serialize for transmission
                serializeJson(wifiData_compl, json);
                // Data to send with HTTP POST 
                int httpResponseCode = http.POST(json); 
                // check response code and emit info led 
                if (httpResponseCode == 200 or httpResponseCode == 201) {
                    digitalWrite(LED_BUILTIN, LOW);
                } else {
                    digitalWrite(LED_BUILTIN, HIGH);
                    Serial.println("Error code: ");
                    Serial.println(httpResponseCode);
                    Serial.println(json);
                }                
                // Free resources for next transmission / Clean Up
                http.end();
                json = "";
                wifiData.clear();
                wifiData_compl.clear();
                delay(submit_delay);  
            }
        }  
    }
    // Delete the scan result to free memory
    Serial.println("Submission done");
    WiFi.scanDelete();
    delay(loop_delay);    
  }
