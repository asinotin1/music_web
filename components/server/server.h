#ifndef __SERVER_H
#define __SERVER_H
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_http_server.h"
#include "driver/i2s_std.h"

typedef struct
{
    i2s_chan_handle_t tx;
    const char *path;
} wav_task_arg_t;

httpd_handle_t start_webserver();

#endif