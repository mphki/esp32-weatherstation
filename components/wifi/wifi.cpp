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

#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "wifi.h"

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"


/*!
 * @brief
 *   WiFi class constructor.
 */
Wifi::Wifi()
{
}


/*!
 * @brief
 *   WiFi class destructor.
 */
Wifi::~Wifi()
{
}


/*! WiFi event group for event_handler */
EventGroupHandle_t wifi_event_group;


/*!
 * @brief
 *   WiFi connection event handler.
 *
 * @param ctx (IN/OUT)
 *   User data.
 *
 * @param event (IN)
 *   Event ID.
 *
 * @return
 *   ESP32 error.
 */
static esp_err_t event_handler(void *ctx, system_event_t *event)
{
    static int retry_num = 0;

    switch(event->event_id)
    {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            retry_num = 0;
            xEventGroupSetBits(wifi_event_group, Wifi::WIFI_CONNECTED_BIT);
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            if (retry_num < CONFIG_ESP_MAXIMUM_RETRY)
            {
                esp_wifi_connect();
                xEventGroupClearBits(wifi_event_group, Wifi::WIFI_CONNECTED_BIT);
                retry_num++;
            }
            break;
        default:
            break;
    }
    return ESP_OK;
}


/*!
 * @brief
 *   Connect to WiFi.
 *
 * @return
 *   True if connected, false otherwise.
 */
bool Wifi::connect() const
{
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    wifi_config_t wifi_config = {CONFIG_ESP_WIFI_SSID, CONFIG_ESP_WIFI_PASSWORD};
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    // Wait until WiFi is connected or timeout or retried maximum times
    EventBits_t uxBits = xEventGroupWaitBits(
            wifi_event_group, WIFI_CONNECTED_BIT, false, true, WIFI_WAIT_TIME_MS / portTICK_RATE_MS);

    // Return true if connected, false otherwise
    return ((uxBits & WIFI_CONNECTED_BIT) != 0);
}


/*!
 * @brief
 *   Disconnect from WiFi.
 */
void Wifi::disconnect() const
{
    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
}
