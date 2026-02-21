#include "server.h"
#include "max98357.h"

extern i2s_chan_handle_t tx;

static const char *TAG = "ESP32_HTTP_SERVER";
volatile bool stop_wav = false;
TaskHandle_t wav_task_handler = NULL;
/*
A: AM THAM BEN EM
B: CON MUA NGANH QUA
C: HONG NHAN
D: BAC PHAN
*/

static esp_err_t Ahandler(httpd_req_t *req)
{

    if (wav_task_handler != NULL)
    {
        stop_wav = true;
        while (wav_task_handler != NULL)
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");

    // TẠO TASK PHÁT NHẠC

    wav_task_arg_t *arg = malloc(sizeof(wav_task_arg_t));
    arg->tx = tx;
    arg->path = "/sdcard/aa.wav";

    xTaskCreatePinnedToCore(wav_play_task, "wav_play_task", 8192, arg, 5, &wav_task_handler, 0);

    const char *mRes = "{\"data\":\"A\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, mRes, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static esp_err_t Bhandler(httpd_req_t *req)
{
    if (wav_task_handler != NULL)
    {
        stop_wav = true;
        while (wav_task_handler != NULL)
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");

    // TẠO TASK PHÁT NHẠC

    wav_task_arg_t *arg = malloc(sizeof(wav_task_arg_t));
    arg->tx = tx;
    arg->path = "/sdcard/bb.wav";

    xTaskCreatePinnedToCore(wav_play_task, "wav_play_task", 8192, arg, 5, &wav_task_handler, 0);
    const char *mRes = "{\"data\":\"B\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, mRes, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}
static esp_err_t Chandler(httpd_req_t *req)
{
    if (wav_task_handler != NULL)
    {
        stop_wav = true;
        while (wav_task_handler != NULL)
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");

    // TẠO TASK PHÁT NHẠC

    wav_task_arg_t *arg = malloc(sizeof(wav_task_arg_t));
    arg->tx = tx;
    arg->path = "/sdcard/cc.wav";

    xTaskCreatePinnedToCore(wav_play_task, "wav_play_task", 8192, arg, 5, &wav_task_handler, 0);
    const char *mRes = "{\"data\":\"C\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, mRes, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}
static esp_err_t Dhandler(httpd_req_t *req)
{
    if (wav_task_handler != NULL)
    {
        stop_wav = true;
        while (wav_task_handler != NULL)
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "*");

    // TẠO TASK PHÁT NHẠC

    wav_task_arg_t *arg = malloc(sizeof(wav_task_arg_t));
    arg->tx = tx;
    arg->path = "/sdcard/dd.wav";

    xTaskCreatePinnedToCore(wav_play_task, "wav_play_task", 8192, arg, 5, &wav_task_handler, 0);
    const char *mRes = "{\"data\":\"D\"}";
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, mRes, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static const httpd_uri_t uri_A = {
    .uri = "/A",
    .method = HTTP_GET,
    .handler = Ahandler,
    .user_ctx = NULL,
};
static const httpd_uri_t uri_B = {
    .uri = "/B",
    .method = HTTP_GET,
    .handler = Bhandler,
    .user_ctx = NULL,
};
static const httpd_uri_t uri_C = {
    .uri = "/C",
    .method = HTTP_GET,
    .handler = Chandler,
    .user_ctx = NULL,
};
static const httpd_uri_t uri_D = {
    .uri = "/D",
    .method = HTTP_GET,
    .handler = Dhandler,
    .user_ctx = NULL,
};

httpd_handle_t start_webserver()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;
    ESP_LOGI(TAG, "Server port '%d'", config.server_port);

    if (httpd_start(&server, &config) == ESP_OK)
    {
        httpd_register_uri_handler(server, &uri_A);
        httpd_register_uri_handler(server, &uri_B);
        httpd_register_uri_handler(server, &uri_C);
        httpd_register_uri_handler(server, &uri_D);
        ESP_LOGI(TAG, "Server starting...!");
    }
    else
    {
        ESP_LOGE(TAG, "Err starting server...!");
    }

    return server;
}