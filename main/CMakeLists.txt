set(INCLUDES "." "./ui/generated" "./ui/custom" "./ui/generated/guider_customer_fonts")
file(GLOB_RECURSE SOURCES ./*.c)
idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS ${INCLUDES}
        REQUIRES st7735
                 lsm6dsv16x
                 esp_timer
                 lv_port
                 lvgl
                 nvs_flash
                 ap_wifi
)
spiffs_create_partition_image(html ../html FLASH_IN_PROJECT)
