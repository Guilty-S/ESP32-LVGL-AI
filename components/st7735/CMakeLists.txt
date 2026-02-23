idf_component_register(
        SRCS "st7735.c"
        INCLUDE_DIRS "."
        REQUIRES fonts driver    # <--- 必须明确告诉编译器：st7735 依赖 fonts 组件
)