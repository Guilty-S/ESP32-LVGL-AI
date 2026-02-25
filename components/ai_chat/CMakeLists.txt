idf_component_register(
        SRCS "ai_chat.c"
        INCLUDE_DIRS "."
        REQUIRES  esp_http_client esp-tls mbedtls json

)