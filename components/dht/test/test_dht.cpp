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

#include <limits.h>
#include "unity.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h "
#include "dht.h"

static const gpio_num_t CORRECT_PORT = GPIO_NUM_25;
static const gpio_num_t INCORRECT_PORT = GPIO_NUM_5;
static const int MEAS_DELAY_MS = 1000;


TEST_CASE("Successful sensor measurement", "[dht]")
{
    DHT dht;
    dht.setDHTgpio(CORRECT_PORT);
	vTaskDelay(MEAS_DELAY_MS / portTICK_RATE_MS);
    int dht_status = dht.readDHT();
    TEST_ASSERT_EQUAL(DHT_OK, dht_status);
    float temperature = dht.getTemperature();
    int humidity = static_cast<int>(dht.getHumidity() + 0.5);
	TEST_ASSERT_FLOAT_WITHIN(30.0, 0.0, temperature);
	TEST_ASSERT_INT_WITHIN(49, 50, humidity);
}


TEST_CASE("Failed sensor measurement, incorret GPIO port", "[dht]")
{
    DHT dht;
    dht.setDHTgpio(INCORRECT_PORT);
	vTaskDelay(MEAS_DELAY_MS / portTICK_RATE_MS);
    int dht_status = dht.readDHT();
    TEST_ASSERT_EQUAL(DHT_TIMEOUT_ERROR, dht_status);
}



