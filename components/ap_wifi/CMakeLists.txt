idf_component_register(
        SRCS "ap_wifi.c" "wifi_manager.c" "ws_server.c"
        INCLUDE_DIRS "."
        REQUIRES driver esp_wifi esp_http_server lwip json spiffs my_sntp
)