#include <cJSON.h>
#include "esp_system.h"
#include "esp_log.h"

#include "esp_http_client.h"
#include "esp_wifi.h"

static const char *TAG = "HTTP_CLIENT";
char open_weather_map_api_key[] = "***************";

char city[] = "Kadikoy";
char country_code[] = "TR";

#define HTTP_RESPONSE_BUFFER_SIZE 1024

char *response_data = NULL;
char *response_data2 = NULL;
size_t response_len = 0;
bool all_chunks_received = false;

char tempS[7];
char humidS[7];
int year, month, day, hour, minute, second;

void get_temp_pressure_humidity(const char *json_string , size_t response_len)
{
   
    cJSON *root = cJSON_Parse(json_string);
    cJSON *obj = cJSON_GetObjectItemCaseSensitive(root, "main");

    float temp = cJSON_GetObjectItemCaseSensitive(obj, "temp")->valuedouble;
    int pressure = cJSON_GetObjectItemCaseSensitive(obj, "pressure")->valueint;
    int humidity = cJSON_GetObjectItemCaseSensitive(obj, "humidity")->valueint;
    printf("Temperature: %0.02f°C\nPressure: %d hPa\nHumidity: %d%%\n", temp, pressure, humidity);

    sprintf(tempS, "%.2f", temp);
    sprintf(humidS, "%d", humidity);
    
    cJSON_Delete(root);
    memset(response_data, 0, response_len);
    //free(response_data);
}

void get_time_date(const char *json_string, size_t response_len)
{
    cJSON *root = cJSON_Parse(json_string);
    char datetime_value[50];
    cJSON *datetime_item = cJSON_GetObjectItemCaseSensitive(root, "datetime");
    strncpy(datetime_value, datetime_item->valuestring, sizeof(datetime_value) - 1);
    printf("DATE: %s\n", datetime_value);
    sscanf(datetime_value, "%d-%d-%dT%d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    ESP_LOGI("WorldTimeAPI", "Year: %d, Month: %d, Day: %d, Hour: %d, Minute: %d, Second: %d", year, month, day, hour, minute, second);
    cJSON_Delete(root);
    memset(response_data2, 0, response_len);
    //free(response_data2);
}

esp_err_t _http_event_handlerWeather(esp_http_client_event_t *evt)
{
    switch (evt->event_id) {
        case HTTP_EVENT_ON_DATA:
            // Resize the buffer to fit the new chunk of data
            response_data = realloc(response_data, response_len + evt->data_len);
            memcpy(response_data + response_len, evt->data, evt->data_len);
            response_len += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            all_chunks_received = true;
            ESP_LOGI("OpenWeatherAPI", "Received data: %s", response_data);
            get_temp_pressure_humidity(response_data,response_len);
            response_len = 0;
            break;
        default:
            break;
    }
    return ESP_OK;
}



#define MAX_RETRY_COUNT 3

esp_err_t _http_event_handlerTime(esp_http_client_event_t *evt)
{
    ESP_LOGI("-","2");
    static int retry_count = 0;
    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGE("HTTP_EVENT", "HTTP_EVENT_ERROR");
            if (retry_count < MAX_RETRY_COUNT) {
                retry_count++;
                ESP_LOGI("HTTP_EVENT", "Retrying... (%d/%d)", retry_count, MAX_RETRY_COUNT);
                esp_http_client_handle_t client = evt->client;
                esp_err_t err = esp_http_client_perform(client);
                if (err == ESP_OK) {
                    ESP_LOGI("HTTP_EVENT", "Retry successful");
                    retry_count = 0; // Başarılı olursa yeniden deneme sayacını sıfırla
                } else {
                    ESP_LOGE("HTTP_EVENT", "Retry failed");
                }
            } else {
                ESP_LOGE("HTTP_EVENT", "Max retry count reached. Giving up.");
            }
            break;
        case HTTP_EVENT_ON_DATA:
            // Resize the buffer to fit the new chunk of data
            response_data2 = realloc(response_data2, response_len + evt->data_len);
            memcpy(response_data2 + response_len, evt->data, evt->data_len);
            response_len += evt->data_len;
            break;
        case HTTP_EVENT_ON_FINISH:
            all_chunks_received = true;
            ESP_LOGI("WorldTimeAPI", "Received data: %s", response_data2);
            get_time_date(response_data2, response_len);
            response_len = 0;
            break;
        default:
            break;
    }
    return ESP_OK;
}


void openweather_api_http(void)
{
    // Set DNS server
    esp_netif_dns_info_t dns_info;
    IP4_ADDR(&dns_info.ip.u_addr.ip4, 8, 8, 8, 8); // Google DNS
    esp_netif_set_dns_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), ESP_NETIF_DNS_MAIN, &dns_info);

    char open_weather_map_url[200];
    snprintf(open_weather_map_url,
             sizeof(open_weather_map_url),
             "%s%s%s%s%s%s%s",
             "http://api.openweathermap.org/data/2.5/weather?q=",
             city,
             ",",
             country_code,
             "&APPID=",
             open_weather_map_api_key,
             "&units=metric&lang=tr");

    printf("OpenWeatherMap URL: %s\n", open_weather_map_url);

    esp_http_client_config_t config = {
        .url = open_weather_map_url,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handlerWeather,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        printf("HTTP GET Status = %d, content_length = %lld\n",
               esp_http_client_get_status_code(client),
               esp_http_client_get_content_length(client));
        printf("HTTP GET request successful\n");
    } else {
        printf("HTTP GET request failed: %s\n", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}




void worldTime_api_http(void)
{
    // Set DNS server
    esp_netif_dns_info_t dns_info;
    IP4_ADDR(&dns_info.ip.u_addr.ip4, 8, 8, 8, 8); // Google DNS
    esp_netif_set_dns_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), ESP_NETIF_DNS_MAIN, &dns_info);

    char worldTimeApi[200];
    snprintf(worldTimeApi,
             sizeof(worldTimeApi),
             "%s%s%s%s",
             "http://worldtimeapi.org/api/timezone/",
             "Europe",
             "/",
             "Istanbul");

    printf("WorldTimeApi URL: %s\n", worldTimeApi);

    esp_http_client_config_t config = {
        .url = worldTimeApi,
        .method = HTTP_METHOD_GET,
        .event_handler = _http_event_handlerTime,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    ESP_LOGI("-","1111");

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        printf("HTTP GET request successful\n");
    } else {
        printf("HTTP GET request failed: %s\n", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

