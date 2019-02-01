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

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"


/*!
 * @brief
 *   Status LED class constructor.
 *
 * @param port_number (IN)
 *   LED GPIO port number.
 *
 * @param time_ms (IN)
 *   Time to blink LED in milliseconds.
 *
 */
Led::Led(gpio_num_t port_number, int time_ms)
{
    port = port_number;
    time = time_ms;
    gpio_pad_select_gpio(port);
    gpio_set_direction(port, GPIO_MODE_OUTPUT);
}


/*!
 * @brief
 *   Status LED class destructor.
 */
Led::~Led()
{
}


/*!
 * @brief
 *   Blink status LED once.
 */
void Led::blink_once() const
{
    gpio_set_level(port, 0);
    vTaskDelay(time / portTICK_RATE_MS);
    gpio_set_level(port, 1);
}


/*!
 * @brief
 *   Blink status LED continuously.
 */
void Led::blink_continuously() const
{
    while(true)
    {
        gpio_set_level(port, 0);
        vTaskDelay(time / portTICK_RATE_MS);
        gpio_set_level(port, 1);
        vTaskDelay(time / portTICK_RATE_MS);
    }
}
