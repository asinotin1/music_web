#include "max98357.h"

i2s_chan_handle_t tx;
extern volatile bool stop_wav;
extern TaskHandle_t wav_task_handler;

void Max_init(MAX_Typedef *max, int I2S_BCK_IO, int I2S_WS_IO, int I2S_DO_IO, int SD_MISO, int SD_MOSI, int SD_CLK, int SD_CS, int sample_rate_hz)
{
    // Config GPIO
    max->I2S_BCK_IO = I2S_BCK_IO;
    max->I2S_WS_IO = I2S_WS_IO;
    max->I2S_DO_IO = I2S_DO_IO;
    max->SD_MISO = SD_MISO;
    max->SD_MOSI = SD_MOSI;
    max->SD_CLK = SD_CLK;
    max->SD_CS = SD_CS;
    max->sample_rate_hz = sample_rate_hz;

    // Init function
    esp_err_t ret;
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = max->SD_MOSI,
        .miso_io_num = max->SD_MISO,
        .sclk_io_num = max->SD_CLK,
    };

    host.max_freq_khz = 8000;
    ret = spi_bus_initialize(host.slot, &bus_cfg, SDSPI_DEFAULT_DMA);
    ESP_ERROR_CHECK(ret);
    sdspi_device_config_t slot_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_cfg.gpio_cs = max->SD_CS;

    esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
        .format_if_mount_failed = false,
        .max_files = 5};
    sdmmc_card_t *card;
    ret = esp_vfs_fat_sdspi_mount("/sdcard", &host, &slot_cfg, &mount_cfg, &card);
    ESP_ERROR_CHECK(ret);

    // I2S Config

    i2s_chan_config_t chan_cfg = {
        .id = 0,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 16,
        .dma_frame_num = 1024,
        .auto_clear = true,
    };

    ret = i2s_new_channel(&chan_cfg, &tx, NULL);

    ESP_ERROR_CHECK(ret);

    i2s_std_config_t std_cfg = {
        .clk_cfg = {
            .sample_rate_hz = max->sample_rate_hz,
            .clk_src = I2S_CLK_SRC_APLL,
            .mclk_multiple = I2S_MCLK_MULTIPLE_256},
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {.mclk = I2S_GPIO_UNUSED, .bclk = max->I2S_BCK_IO, .ws = max->I2S_WS_IO, .dout = max->I2S_DO_IO, .din = I2S_GPIO_UNUSED},
    };

    ret = i2s_channel_init_std_mode(tx, &std_cfg);
    ESP_ERROR_CHECK(ret);
    ESP_ERROR_CHECK(i2s_channel_enable(tx));

    // xTaskCreatePinnedToCore(wav_play_task, "wav_play_task", 8192, tx, 5, NULL, 0);
}

// Con trỏ void
// Trỏ đến cái gì cũng được

void wav_play_task(void *arg)
{
    wav_task_arg_t *task_arg = (wav_task_arg_t *)arg;
    i2s_chan_handle_t tx = task_arg->tx;
    ESP_LOGI("ESP", "%s", task_arg->path);
    FILE *f = fopen(task_arg->path, "rb");
    if (!f)
    {
        ESP_LOGE("ESP", "Cannot open the file");
        vTaskDelete(NULL);
    }
    wav_header_t header;
    fread(&header, sizeof(header), 1, f);
    // ESP_LOGI("ESP", "Sample rate %s" PRIu32 "channels %s" PRIu32, header.sample_rate, header.channels);

    uint8_t *buf[2];
    buf[0] = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    buf[1] = heap_caps_malloc(BUF_SIZE, MALLOC_CAP_DMA);
    if (!buf[0] || !buf[1])
    {
        ESP_LOGE("ESP", "Cannot heap_caps_malloc");
        fclose(f);
        vTaskDelete(NULL);
    }

    int current = 0;
    size_t bytes_read, byte_written;
    bytes_read = fread(buf[0], 1, BUF_SIZE, f);

    int16_t *stereo = NULL;
    if (header.channels == 1)
    {
        stereo = heap_caps_malloc(BUF_SIZE * 2, MALLOC_CAP_DMA);
        if (!stereo)
        {
            ESP_LOGE("WAV", "Cant malloc stereo buffer");
            free(buf[1]);
            free(buf[0]);
            fclose(f);
            free(task_arg);
            free(stereo);
            vTaskDelete(NULL);
            return;
        }
    }

    while (bytes_read > 0)
    {
        if (stop_wav)
        {
            ESP_LOGE("WAV", "stop_wav");
            break;
        }

        size_t next_bytes = fread(buf[1 - current], 1, BUF_SIZE, f);
        // 1 kênh
        if (header.channels == 1)
        {
            int16_t *mono = (int16_t *)buf[current];

            int samples = bytes_read / 2;
            for (int i = 0; i < samples; i++)
            {
                stereo[2 * i] = mono[i];
                stereo[2 * i + 1] = mono[i];
            }
            i2s_channel_write(tx, stereo, bytes_read * 2, &byte_written, portMAX_DELAY);
        }
        else
        {
            // 2 kênh
            i2s_channel_write(tx, buf[current], bytes_read, &byte_written, portMAX_DELAY);
        }
        current = 1 - current;
        bytes_read = next_bytes;
    }

    stop_wav = false;
    free(buf[0]);
    free(buf[1]);
    free(task_arg);
    free(stereo);
    fclose(f);
    wav_task_handler = NULL;
    vTaskDelete(NULL);
}