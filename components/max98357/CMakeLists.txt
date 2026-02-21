idf_component_register(SRCS "max98357.c"
                    INCLUDE_DIRS "."
                    REQUIRES esp_driver_i2s esp_driver_sdspi fatfs server
                    )
