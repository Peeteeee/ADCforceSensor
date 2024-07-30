#include <inttypes.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <hx711.h>
#include "freertos/semphr.h"
static const char *TAG = "hx711-example";
int32_t ofsetek = 0;
// #define tlacitko GPIO_NUM_22
// SemaphoreHandle_t semaforTestTlacitko = NULL;
// SemaphoreHandle_t semaforTlacitkoTare = NULL;

void idling(void *pvParameter)
{
    while (1)
    {
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}
void idling2(void *pvParameter)
{
    while (1)
    {
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}

// void tare(void *pvParameters)
// {
//     hx711_t dev = {
//         .dout = 19,
//         .pd_sck = 18,
//         .gain = HX711_GAIN_A_64
//     };

//     // initialize device
//     ESP_ERROR_CHECK(hx711_init(&dev));

//     while(1)
//     {
//         //taruji
//         if (xSemaphoreTake(semaforTestTlacitko, portMAX_DELAY) == pdTRUE)
//         {
//             esp_err_t r = hx711_wait(&dev, 5000);
//             if (r != ESP_OK)
//             {
//                 ESP_LOGE(TAG, "Device not found... tarefce: %d (%s)\n", r, esp_err_to_name(r));
//                 continue;
//             }
//             ESP_LOGI(TAG, "Nuluji...   cekej");

//             int32_t data;
//             r = hx711_read_median(&dev, 40, &data);//musi byt sude
//             if (r != ESP_OK)
//             {
//                 ESP_LOGE(TAG, "Could not read data... tarefce: %d (%s)\n", r, esp_err_to_name(r));
//                 continue;
//             }
//             ofsetek = data;
//             ESP_LOGI(TAG, "V poradku, muzes vazit.");
//             vTaskDelay(10/portTICK_PERIOD_MS);
//             xSemaphoreGive(semaforTestTlacitko);
//             vTaskDelete(NULL);
//         }
//         //dotarovano
//     }
// }

// void stisknuteTlacitko(void *pvParameters)
// {
//     while(1)
//     {
//         if(gpio_get_level(tlacitko) == 1)
//         {
//             if (xSemaphoreTake(semaforTestTlacitko, portMAX_DELAY) == pdTRUE)
//             {
//                 xTaskCreatePinnedToCore(tare, "tare", 2048, NULL, 4, NULL, 0);
//                 xSemaphoreGive(semaforTestTlacitko);
//             }
//         }
//         vTaskDelay(10/portTICK_PERIOD_MS);
//     }
// }

void test(void *pvParameters)
{
    hx711_t dev = {
        .dout = 19,
        .pd_sck = 18,
        .gain = HX711_GAIN_B_32};

    // initialize device
    ESP_ERROR_CHECK(hx711_init(&dev));

    while (1)
    {
        // ESP_LOGE(TAG, "test task in while");
        // Wait for the semaphore to be given (signaled)
        //  if (xSemaphoreTake(semaforTestTlacitko, portMAX_DELAY) == pdTRUE)
        //  {
        // ESP_LOGE(TAG, "test task in while in if");
        //  read from device
        esp_err_t r = hx711_wait(&dev, 200);
        if (r != ESP_OK)
        {
            ESP_LOGE(TAG, "Device not found... testfce: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }
        ESP_LOGI(TAG, "Nuluji...   cekej");

        int32_t data = 0;
        r = hx711_read_median(&dev, 40, &data); // musi byt sude
        if (r != ESP_OK)
        {
            ESP_LOGE(TAG, "Could not read data... tarefce: %d (%s)\n", r, esp_err_to_name(r));
            continue;
        }
        ofsetek = data;
        ESP_LOGI(TAG, "V poradku, muzes vazit.");
        vTaskDelay(10 / portTICK_PERIOD_MS);

        while (1)
        {
            data = 0;
            r = hx711_read_median(&dev, 6, &data); // musi byt sude
            if (r != ESP_OK)
            {
                ESP_LOGE(TAG, "Could not read data... testfce: %d (%s)\n", r, esp_err_to_name(r));
                continue;
            }

            double novaData = (double)data - (double)ofsetek;

            ESP_LOGI(TAG, "Data = %d\n", (int)data);
            ESP_LOGI(TAG, "Hmotnost: %.2f grams.\n" PRIi32, (novaData / 9100 < 0 ? -novaData / 9100 : novaData / 9100));
            // xSemaphoreGive(semaforTestTlacitko);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}

void app_main()
{
    // semaforTestTlacitko = xSemaphoreCreateMutex();
    // if (semaforTestTlacitko == NULL) {
    //    ESP_LOGE(TAG, "Semaphore creation failed");
    // }
    // else {ESP_LOGE(TAG, "Semaphore creation succesfull");}

    // semaforTlacitkoTare = xSemaphoreCreateBinary();
    // if (semaforTlacitkoTare == NULL) {
    //    ESP_LOGE(TAG, "Semaphore creation failed");
    // }
    // else {ESP_LOGE(TAG, "Semaphore creation succesfull");}

    // gpio_set_direction(tlacitko, GPIO_MODE_INPUT);
    // gpio_set_pull_mode(tlacitko, GPIO_PULLDOWN_ENABLE);
    // xTaskCreatePinnedToCore(stisknuteTlacitko, "stisknuteTlacitko", 2048, NULL, 6, NULL, 0);
    xTaskCreatePinnedToCore(test, "test", 4096, NULL, 5, NULL, 0);
    xTaskCreatePinnedToCore(idling, "idling", 2048, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(idling2, "idling2", 2048, NULL, 1, NULL, 1);
}