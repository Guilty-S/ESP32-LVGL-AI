# 1. 递归获取所有子目录下的 .c 源文件
file(GLOB_RECURSE SOURCES "generated/*.c" "custom/*.c" "*.c")

# 2. 注册组件
idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS
        "."
        "generated"
        "custom"
        REQUIRES
        lvgl
        lv_port
        esp_lvgl_port
        ai_chat
)