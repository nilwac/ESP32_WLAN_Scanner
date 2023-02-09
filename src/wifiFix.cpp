#include <WiFi.h>
#include <lwip/sockets.h>

#define WIFI_CLIENT_FLUSH_BUFFER_SIZE    (1024)

class WiFiClientFixed : public WiFiClient {
public:
    void flush() override;
};

void WiFiClientFixed::flush() {
    int res;
    size_t a = available(), toRead = 0;
    if (!a) {
        return;//nothing to flush
    }
    uint8_t *buf = (uint8_t *) malloc(WIFI_CLIENT_FLUSH_BUFFER_SIZE);
    if (!buf) {
        return;//memory error
    }
    while (a) {
        toRead = (a > WIFI_CLIENT_FLUSH_BUFFER_SIZE) ? WIFI_CLIENT_FLUSH_BUFFER_SIZE : a;
        // override broken WiFiClient flush method, ref https://github.com/espressif/arduino-esp32/issues/6129#issuecomment-1237417915
        //res = recv(fd(), buf, toRead, MSG_DONTWAIT);
        res = read(buf, a);
        if (res < 0) {
            log_e("fail on fd %d, errno: %d, \"%s\"", fd(), errno, strerror(errno));
            stop();
            break;
        }
        a -= res;
    }
    free(buf);
}