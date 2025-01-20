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
#include "bitmaps/images/personaBWR2.h"

#include "bitmaps/images/testBMPData.h"
#include "bitmaps/images/rainy26x26.h"

#include "bitmaps/fonts/FreeMonoBold12pt7b.h"
#include "bitmaps/fonts/fontYeni.h"


void periodic_task(void *pvParameters)
{
    epaper_wait_seconds(5);
    while (1)
    {
        check_wifi_connection();
        openweather_api_http();
        epaper_wait_seconds(1);
        worldTime_api_http();
        epaper_reset();
        epaper_bw_buffer_clear();
        epaper_red_buffer_clear();
        draw_word_withFont_color("Weather Info", 10, 15, &FreeMonoBold12pt7b, RED);

        char text[25];
        sprintf(text, "T:%sºC", tempS);

        draw_word_withFont_color(text, 10, 45, &FreeMonoBold12pt7bY, BLACK);

        sprintf(text, "H:%s%%", humidS);
        draw_word_withFont_color(text, 10, 65, &FreeMonoBold12pt7bY, BLACK);

        draw_line_horizontal(10, 240, 80);

        sprintf(text, "%d.%d.%d", day, month, year);
        draw_word_withFont_color(text, 10, 105, &FreeMonoBold12pt7bY, BLACK);

        draw_line_vertical(140, 90, 110);

        sprintf(text, "%d:%d", hour, minute);
        draw_word_withFont_color(text, 155, 105, &FreeMonoBold12pt7bY, RED);

        epaper_writeBufferToDisplay();
        epaper_update();
        epaper_deep_sleep();
        // Her 60 saniyede bir çalışacak kodu buraya ekleyin
        printf("Task is running every 60 seconds\n");
        vTaskDelay(60000 / portTICK_PERIOD_MS); // 60 saniye bekle
    }
}


void app_main(void)
{
    size_t free_heap_size = xPortGetFreeHeapSize();
    printf("Free heap size opening: %d bytes\n", free_heap_size);
    nvs_flash_init();

    epaper_init();
    epaper_clear();
    epaper_writeBufferToDisplay();
    epaper_update();
    wifi_connection();
    wait_for_wifi_connection();

    //epaper_draw_blackAndRedBitmaps(personaBWR2_BLACK, personaBWR2_RED);

    epaper_bw_buffer_clear();
    epaper_red_buffer_clear();
    epaper_draw_partial_blackAndRedBitmapsEnhanced(jgirl60x60_BLACK, jgirl60x60_RED, 190, 0, 60, 60, 60, 60);  
    epaper_draw_partial_blackAndRedBitmapsEnhanced(rainy26x26_BLACK,rainy26x26_BLACK, 0, 0, 26, 26, 26, 26);
    epaper_draw_partial_blackAndRedBitmapsEnhanced(testBMPData_BLACK, testBMPData_RED, 155, 50, 40, 40, 40, 40);
    draw_line_horizontal(150, 240, 49);
    draw_line_horizontal(150, 240, 91);
    draw_line_vertical(154, 40, 100);
    draw_line_vertical(196, 40, 100);
    epaper_writeBufferToDisplay();
    epaper_update();

    epaper_wait_seconds(10);

    xTaskCreate(&periodic_task, "periodic_task", 4096, NULL, 5, NULL);
    free_heap_size = xPortGetFreeHeapSize();
    printf("Free heap size after task creation: %d bytes\n", free_heap_size);
    /*
    epaper_wait_seconds(50);

    openweather_api_http();
    epaper_wait_seconds(1);
    worldTime_api_http();
    
    epaper_wait_seconds(5);
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

    draw_line_vertical(140, 90, 110);

    sprintf(text, "%d:%d", hour, minute);
    draw_word_withFont_color(text, 155, 105, &FreeMonoBold12pt7bY, RED);

    epaper_writeBufferToDisplay();
    epaper_update();

    epaper_deep_sleep();

    epaper_wait_seconds(5);
    epaper_reset();
    epaper_update();
    free_heap_size = xPortGetFreeHeapSize();
    printf("Free heap size after: %d bytes\n", free_heap_size);
    //xTaskCreate(&periodic_task, "periodic_task", 2048, NULL, 5, NULL);
    */
}

