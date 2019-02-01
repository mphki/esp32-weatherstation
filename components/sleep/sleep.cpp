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

#include <soc/rtc.h>
#include "esp_sleep.h"
#include "sleep.h"


/*!
 * @brief
 *   Deep sleep class constructor.
 *   Gets boot time.
 */
Sleep::Sleep()
{
    start_time_us = rtc_time_get();
}


/*!
 * @brief
 *   Deep sleep class destructor.
 */
Sleep::~Sleep()
{
}


/*!
 * @brief
 *   Go to deep sleep for the given interval (reboots after this).
 *
 * @param interval_min (IN)
 *   Time to deep sleep in minutes.
 */
void Sleep::deep_sleep(int interval_min) const
{
    uint64_t interval_us = MIN_TO_US * static_cast<uint64_t>(interval_min);
    uint64_t sleep_time_us = interval_us - (rtc_time_get() - start_time_us) - TIME_TO_BOOT_US;
    esp_sleep_enable_timer_wakeup(sleep_time_us);
    esp_deep_sleep_start();
};
