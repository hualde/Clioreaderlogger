#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "config.h"
#include <string.h>
#include "esp_mac.h"

static const char *WEBSERVER_TAG = "webserver";

httpd_handle_t server = NULL;

#define LOG_BUFFER_SIZE 4096
static char log_buffer[LOG_BUFFER_SIZE];
static int log_buffer_index = 0;

static void add_to_log_buffer(const char *log_message) {
    size_t len = strlen(log_message);
    if (log_buffer_index + len >= LOG_BUFFER_SIZE) {
        log_buffer_index = 0;
    }
    strncpy(log_buffer + log_buffer_index, log_message, LOG_BUFFER_SIZE - log_buffer_index - 1);
    log_buffer_index += len;
    log_buffer[log_buffer_index++] = '\n';
    log_buffer[LOG_BUFFER_SIZE - 1] = '\0';
}

static void log_output_func(const char *fmt, va_list args) {
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, args);
    add_to_log_buffer(buf);
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    char *html_start = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>ESP32 Webserver</title><style>body{font-family:Arial,sans-serif;line-height:1.6;padding:20px;max-width:800px;margin:0 auto;background-color:#f4f4f4}h1{color:#333}pre{background-color:#e4e4e4;padding:10px;border-radius:5px;white-space:pre-wrap;word-wrap:break-word}.info{background-color:#d4edda;border-color:#c3e6cb;color:#155724;padding:10px;border-radius:5px;margin-bottom:20px}.logs{background-color:#fff;border:1px solid #ddd;padding:10px;border-radius:5px;max-height:400px;overflow-y:auto}</style></head><body><h1>ESP32 Webserver</h1>";
    char *html_end = "</body></html>";

    char *resp_str = malloc(LOG_BUFFER_SIZE + 2048); // Increased buffer size for HTML
    if (resp_str == NULL) {
        ESP_LOGE(WEBSERVER_TAG, "Failed to allocate memory for response");
        return ESP_ERR_NO_MEM;
    }

    int written = snprintf(resp_str, LOG_BUFFER_SIZE + 2048,
             "%s<div class=\"info\"><strong>VIN del vehiculo:</strong> %s<br><strong>VIN de la columna:</strong> %s</div><h2>Logs:</h2><pre class=\"logs\">%s</pre>%s",
             html_start, vin_vehiculo, vin_columna, log_buffer, html_end);

    if (written >= LOG_BUFFER_SIZE + 2048) {
        ESP_LOGW(WEBSERVER_TAG, "Response truncated");
    }

    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, resp_str, strlen(resp_str));
    free(resp_str);
    return ESP_OK;
}

static const httpd_uri_t root = {
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = root_get_handler,
    .user_ctx  = NULL
};

void start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    ESP_LOGI(WEBSERVER_TAG, "Starting server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(WEBSERVER_TAG, "Registering URI handlers");
        httpd_register_uri_handler(server, &root);
    }
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(WEBSERVER_TAG, "Station "MACSTR" joined, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(WEBSERVER_TAG, "Station "MACSTR" left, AID=%d",
                 MAC2STR(event->mac), event->aid);
    }
}

void wifi_init_softap(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        NULL));

    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_SSID,
            .ssid_len = strlen(WIFI_SSID),
            .channel = 1,
            .password = WIFI_PASS,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    if (strlen(WIFI_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(WEBSERVER_TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
             WIFI_SSID, WIFI_PASS, 1);

    esp_log_set_vprintf(log_output_func);

    start_webserver();
}