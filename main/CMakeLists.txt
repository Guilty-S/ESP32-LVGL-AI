set(INCLUDES "." "./ui/generated" "./ui/custom" "./ui/generated/guider_customer_fonts" )
file(GLOB_RECURSE SOURCES ./*.c)
file(GLOB_RECURSE UI_SOURCES "ui/generated/*.c")
idf_component_register(
        SRCS ${SOURCES}
             ${UI_SOURCES}
        INCLUDE_DIRS ${INCLUDES}
        REQUIRES
        st7735
        ai_chat
        weather 
        esp_timer  
        lv_port   
        lvgl
        nvs_flash
        ap_wifi 
        esp_lvgl_port
        esp_http_client 
        mbedtls 
        esp-tls 
) 
spiffs_create_partition_image(html ../html FLASH_IN_PROJECT)
spiffs_create_partition_image(img ../img FLASH_IN_PROJECT)
