idf_component_register(
        SRCS "weather.c"
        INCLUDE_DIRS "."
        REQUIRES
        ap_wifi
        esp_http_client
        mbedtls
        esp-tls
        json

)