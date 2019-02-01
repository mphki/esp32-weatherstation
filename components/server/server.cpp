/*
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

#include <string>
#include <sstream>
#include <iomanip>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_http_client.h"
#include "server.h"

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"


/*!
 * @brief
 *   Server class constructor.
 *
 * @param server_get_address (IN)
 *   Server address to get measurement interval.
 *
 * @param server_post_address (IN)
 *   Server address to post temperature and humidity.
 */
Server::Server(std::string server_get_address, std::string server_post_address)
{
    get_address = server_get_address;
    post_address = server_post_address;
    client = nullptr;
}


/*!
 * @brief
 *   Server class destructor.
 */
Server::~Server()
{
}


/*!
 * @brief
 *   HTTP connection event handler.
 *
 * @param evt (IN)
 *   HTTP client events data.
 *
 * @return
 *   ESP32 error.
 */
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    return ESP_OK;
}


/*!
 * @brief
 *   Connect to server.
 *
 * @return
 *   True if connected, false otherwise.
 */
bool Server::connect()
{
    esp_http_client_config_t config = {get_address.c_str()};
    config.event_handler = http_event_handler;
    client = esp_http_client_init(&config);
    esp_http_client_perform(client);
    int status_code = esp_http_client_get_status_code(client);

    return (status_code == HTTP_OK);
}


/*!
 * @brief
 *   Disconnect from server.
 */
void Server::disconnect()
{
    esp_http_client_cleanup(client);
}


/*!
 * @brief
 *   Get measurement interval from server file.
 *
 * @param interval_min (OUT)
 *   Measurement interval in minutes.
 *
 * @return
 *   True if reading succeeds, false otherwise.
 */
bool Server::get_interval(int &interval_min)
{
    int content_length = esp_http_client_get_content_length(client);
    char buffer[READ_BUFFER_SIZE];
    int read_len = esp_http_client_read(client, buffer, content_length);
    buffer[read_len] = 0;
    interval_min = atoi(buffer);
    int status_code = esp_http_client_get_status_code(client);

    return (status_code == HTTP_OK);
}


/*!
 * @brief
 *   Post sensor data to server.
 *
 * @param temperature (IN)
 *   Temperature value to post.
 *
 * @param humidity (IN)
 *   Humidity value to post.
 *
 * @return
 *   True if posting succeeds, false otherwise.
 */
bool Server::post_sensor_data(float temperature, int humidity)
{
    std::stringstream post_data;
	post_data << std::fixed << std::setprecision(1);
    post_data << TEMPERATURE_ID << temperature << HUMIDITY_ID << humidity;
    std::string data = post_data.str();
    esp_http_client_set_url(client, post_address.c_str());
    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_post_field(client, data.c_str(), data.length());
    esp_http_client_perform(client);
    int status_code = esp_http_client_get_status_code(client);

    return (status_code == HTTP_OK);
}
