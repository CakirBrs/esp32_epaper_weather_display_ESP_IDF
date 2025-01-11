#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <stdio.h> //for basic printf commands
#include <string.h> //for handling strings
#include "esp_system.h" //esp_init funtions esp_err_t 
#include "esp_wifi.h" //esp_wifi_init functions and wifi operations
#include "esp_log.h" //for showing logs
#include "esp_event.h" //for wifi event
#include "nvs_flash.h" //non volatile storage
#include "lwip/err.h" //light weight ip packets error handling
#include "lwip/sys.h" //system applications for light weight ip apps
#include "freertos/FreeRTOS.h" //for delay,mutexs,semphrs rtos operations


extern int retry_num;


void wifi_connection(void);
int check_wifi_status(void);
void wait_for_wifi_connection();

#endif // WIFI_CONNECTION_H