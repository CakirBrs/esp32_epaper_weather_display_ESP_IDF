#include <stdio.h> //for basic printf commands
#include <string.h> //for handling strings
#include "freertos/FreeRTOS.h" //for delay,mutexs,semphrs rtos operations
#include "nvs_flash.h" //non volatile storage
#include "freertos/task.h"
#include "esp_system.h"


#include "wifi_connection.h" //for wifi_connection function
#include "httpRequest.h" //for openweather_api_http function
#include "epaper.h" //for epaper functions

//bitmaps
#include "bitmaps/images/jgirl60x60.h"
#include "bitmaps/fonts/FreeMonoBold12pt7b.h"
#include "bitmaps/fonts/fontYeni.h"

void app_main(void)
{
    nvs_flash_init();
    wifi_connection();
    wait_for_wifi_connection();
    openweather_api_http();
    worldTime_api_http();
    epaper_init();

    epaper_draw_partial_blackAndRedBitmapsEnhanced(jgirl60x60_BLACK, jgirl60x60_RED, 190, 0, 60, 60, 60, 60);  
    epaper_writeBufferToDisplay();
    epaper_update();

    epaper_wait_seconds(1);
    debugB = 0; //if you want to see the debug prints on monitor, set it to 1
    draw_word_withFont_color("Weather Info", 10, 15, &FreeMonoBold12pt7b, RED);

    char text[25];
    sprintf(text, "T:%sºC", tempS);

    draw_word_withFont_color(text, 10, 45, &FreeMonoBold12pt7bY, BLACK);

    sprintf(text, "H:%s%%", humidS);
    draw_word_withFont_color(text, 10, 65, &FreeMonoBold12pt7bY, BLACK);

    draw_line_horizontal(10, 240, 80);

    sprintf(text, "%d.%d.%d", day, month, year);
    draw_word_withFont_color(text, 10, 105, &FreeMonoBold12pt7bY, BLACK);

    sprintf(text, "%d:%d", hour, minute);
    draw_word_withFont_color(text, 155, 105, &FreeMonoBold12pt7bY, RED);

    epaper_writeBufferToDisplay();
    epaper_update();

    epaper_deep_sleep();

}

