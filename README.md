# TemperatureTracker
ESP8266 Temperature tracker that records results in Adafruit IO time series database

1. Create a file named config.h with the following:
```c
#define AIO_USERNAME   "aio_username_from_io.adafruit.com"
#define AIO_KEY        "aio_key_from_io.adafruit.com"
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define WIFI_SSID   "wifi_ssid"
#define WIFI_PASS   "wifi_password"
```
2. Create the feeds you want through Adafruit's web UI and update the names in the feeds array. 
