set(src_dirs
    LED_Device
    Key_Device
    RGB_Device
)

set(include_dirs
    LED_Device
    Key_Device
    RGB_Device
)

set(requires
    driver
    led_strip
)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})
component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)