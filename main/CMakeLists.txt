idf_component_register(
    SRCS "hello_world_main.c"
    PRIV_REQUIRES
        spi_flash
        esp_wifi
        nvs_flash
        esp_event
        esp_netif
        esp_driver_uart
        esp_driver_gpio
    INCLUDE_DIRS ""
)