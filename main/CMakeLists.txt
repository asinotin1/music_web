idf_component_register(SRCS "MUSIC.c"
                    INCLUDE_DIRS "."
                    REQUIRES driver esp_wifi nvs_flash esp_http_server server max98357)
