idf_component_register(SRCS "server.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_i2s esp_driver_sdspi esp_http_server max98357
                    )
