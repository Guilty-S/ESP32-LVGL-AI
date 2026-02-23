idf_component_register(
        SRCS "lv_port.c"
        INCLUDE_DIRS "."
        REQUIRES lvgl driver log st7735    # <--- 必须明确告诉编译器：st7735 依赖 fonts 组件
)