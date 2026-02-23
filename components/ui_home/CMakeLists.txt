idf_component_register(
        SRCS "ui_home.c"
        INCLUDE_DIRS "."
        REQUIRES lvgl lv_port st7735 lsm6dsv16x
)