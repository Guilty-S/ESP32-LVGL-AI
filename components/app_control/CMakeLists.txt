idf_component_register(
        SRCS "app_control.c"
        INCLUDE_DIRS "."
        REQUIRES
        esp_timer
        driver
        ap_wifi
        ai_chat
        lvgl
        my_button
        esp_lvgl_port
)