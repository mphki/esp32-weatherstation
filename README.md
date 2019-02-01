# Weather station for ESP32
ESP-IDF C++ project to read DHT22 temperature/humidity sensor and send data to a web page.

- Measurement values are sent to a web page via WiFi.
- After measurement ESP32 goes to deep sleep for an interval to minimize power consumption. The interval length can be given in the web page.
- When the interval has passed ESP32 reboots to do another measurement.
- Temperature and humidity history is shown graphically in the the web page. See http://www.tempes.com/weather.php for an example.
- DHT22 sensor driver is from https://github.com/gosouth/DHT22-cpp.

### Configure the project

In both current and `/test` directory, run `make menuconfig` and set your Wifi SSID, password and number of connection retries in `Example Configuration`. Change `SERVER_ADDRESS` in `weather_main.cpp` and `test_server.cpp` to your web page address.

### Build and flash weather station

Run `make -j4 flash` in the current directory.

### Build and flash unit tests

Run `make -j4 flash monitor` in `/test` directory.

### Setup web page

Copy PHP graphics library from http://www.goat1000.com/svggraph.php  to your web page into folder /SVGGraph. Copy files from `/server_files` to your web page. The file `weather.php` shows the data.

