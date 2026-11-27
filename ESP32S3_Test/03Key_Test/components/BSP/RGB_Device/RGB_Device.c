#include "RGB_Device.h"

static const char *TAG = "rgb_example";
static led_strip_handle_t led_strip;

void RGB_Init(void)
{
    led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_GPIO_PIN,
        .max_leds = LED_STRIP_LED_COUNT,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,
        .flags = { .invert_out = false },
    };

    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000, // 10MHz，1 tick = 0.1us
        .flags = { .with_dma = false },
    };

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "LED strip initialized on GPIO %d", RGB_GPIO_PIN);
}

void RGB_Toggle(void)
{
    static bool is_white = true;

    if (is_white) 
    {
       led_strip_set_pixel(led_strip, 0, 255, 255, 255);
    }
    else 
    {
        led_strip_set_pixel(led_strip, 0, 0, 0, 0);
    }
        led_strip_refresh(led_strip); // 必须调用 refresh 才会真正输出到灯珠

        is_white = !is_white;
    
}

