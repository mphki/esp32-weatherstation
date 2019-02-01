/*
 * Weather station for ESP32 using DHT22 temperature and humidity sensor.
 *
 * - Reads measurement interval from web server.
 * - Measures temperature and humidity with DHT22.
 * - Sends measured data to web server via WiFi.
 * - Goes to deep sleep for the interval and reboots to repeat the above.
 * - Web server shows temperature and humidity history graphically.
 *
 * Uses DHT22 driver from https://github.com/gosouth/DHT22-cpp.
 *
 * MIT License
 *
 * Copyright (c) 2019 Matti Paakko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

/*! @file */

#include <string>
#include "freertos/FreeRTOS.h"
#include "nvs_flash.h"
#include "dht.h"
#include "led.h"
#include "wifi.h"
#include "server.h"
#include "sleep.h"


/*! HOW TO CONFIGURE WEATHER STATION:
 * - Set WiFi SSID, password and number of connection retries with ESP-IDF
 *   "make menuconfig" in "Example Configuration".
 * - Set SERVER_ADDRESS below and copy the files from /server_files
 *   to SERVER_ADDRESS.
 * - Copy PHP graphics library from http://www.goat1000.com/svggraph.php
 *   to SERVER_ADDRESS/SVGGraph/.
 * - You can view temperature/humidity history in SERVER_ADDRESS/weather.php.
 */

/*! Server for data collection and display */
static const std::string SERVER_ADDRESS = "https://your.website.address/";

/*! Server file address to read measurement interval */
static const std::string GET_ADDRESS = SERVER_ADDRESS + "interval.txt";

/*! Server file address to collect sensor data */
static const std::string POST_ADDRESS = SERVER_ADDRESS + "collect.php";

/*! Default measurement interval in minutes */
static const int DEFAULT_INTERVAL_MIN = 10;

/*! DHT22 delay time in milliseconds */
static const int DHT_DELAY_MS = 1000;

/*! LED blink time in milliseconds */
static const int LED_BLINK_TIME_MS = 300;

/*! Number of sensor reading retries */
static const int NUM_SENSOR_READ_RETRIES = 5;

/*! Number of measurement retries */
static const int NUM_MEASUREMENT_RETRIES = 3;

/*! GPIO port for DHT22 temperature and humidity sensor */
static const gpio_num_t DHT_PORT = GPIO_NUM_25;

/*! GPIO port for status LED */
static const gpio_num_t LED_PORT = GPIO_NUM_16;


/*!
 * @brief
 *   Read temperature and humidity from DHT22 sensor.
 *   Retry sensor reading if fails.
 *   Round humidity value to integer as humidity accuracy is +/- 2 %.
 *
 * @param temperature (OUT)
 *   Measured temperature in degrees Celsius.
 *
 * @param humidity (OUT)
 *   Measured humidity in percent.
 *
 * @return
 *   True if sensor reading succeeds, false otherwise.
 */
static bool read_sensor_data(float &temperature, int &humidity)
{
    DHT dht;
    dht.setDHTgpio(DHT_PORT);
    int counter = 0;
    int dht_status = DHT_TIMEOUT_ERROR;

    while ((dht_status != DHT_OK) && (counter < NUM_SENSOR_READ_RETRIES))
    {
        vTaskDelay(DHT_DELAY_MS / portTICK_RATE_MS);
        dht_status = dht.readDHT();
        counter++;
    }

    temperature = dht.getTemperature();
    humidity = static_cast<int>(dht.getHumidity() + 0.5);

    return (dht_status == DHT_OK);
}


/*!
 * @brief
 *   Read interval from server, read sensor data and send it to server.
 *
 * @param interval_min (OUT)
 *   Measurement interval from server file in minutes.
 *
 * @return
 *   True if sensor reading and data sending succeeds, false otherwise.
 */
static bool read_send(int &interval_min)
{
    bool data_ok = false;
    Server server(GET_ADDRESS, POST_ADDRESS);
    bool server_ok = server.connect();

    if (server_ok)
    {
        server_ok = server.get_interval(interval_min);

        if (server_ok)
        {
            float temperature;
            int humidity;
            data_ok = read_sensor_data(temperature, humidity);

            if (data_ok)
            {
                server_ok = server.post_sensor_data(temperature, humidity);
            }
        }
        else
        {
            interval_min = DEFAULT_INTERVAL_MIN;
        }
    }

    server.disconnect();

    return (data_ok && server_ok);
}


/*!
 * @brief
 *   Connect to WiFi, do measurement and go to deep sleep to conserve power.
 *   Retry measurement if fails.
 *   Blink status LED once at boot and continuously if WiFi connection fails.
 */
static void measure(void)
{
    Sleep sleep;
    Led status_led(LED_PORT, LED_BLINK_TIME_MS);
    status_led.blink_once();
    Wifi wifi;

    if (wifi.connect())
    {
        int interval_min;
        bool ok = false;
        int counter = 0;

        while (!ok && (counter < NUM_MEASUREMENT_RETRIES))
        {
            ok = read_send(interval_min);
            counter++;
        }

        wifi.disconnect();
        sleep.deep_sleep(interval_min);
    }
    else
    {
        status_led.blink_continuously();
    }
}


/*!
 * @brief
 *   Application start.
 *   Initialize non-volatile storage.
 *   Perform one temperature/humidity measurement.
 */
extern "C" void app_main()
{
    nvs_flash_init();
    measure();
}
