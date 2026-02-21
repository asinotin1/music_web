#ifndef _MAX98357_
#define _MAX98357_

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "math.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "server.h"

//----------------------------------STRUCT-------------------------------------
typedef struct
{
    int I2S_BCK_IO;
    int I2S_WS_IO;
    int I2S_DO_IO;
    int SD_MISO;
    int SD_MOSI;
    int SD_CLK;
    int SD_CS;
    int sample_rate_hz;

} MAX_Typedef;

typedef struct __attribute__((packed))
{
    char riff[4];
    uint32_t file_size;
    char wave[4];
    char fmt[4];
    uint32_t fmt_len;
    uint16_t format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits;
    char data[4];
    uint32_t data_bytes;
} wav_header_t;

#define BUF_SIZE (16 * 1024) // 16KB

void Max_init(MAX_Typedef *max, int I2S_BCK_IO, int I2S_WS_IO, int I2S_DO_IO, int SD_MISO, int SD_MOSI, int SD_CLK, int SD_CS, int sample_rate_hz);

void wav_play_task(void *arg);

#endif