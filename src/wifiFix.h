#include <WiFi.h>

class WiFiClientFixed : public WiFiClient {
public:
    void flush() override;
};