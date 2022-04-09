//
// Created by vaca on 4/9/22.
//

#include "my_ota.h"
#include "esp_ota_ops.h"
#include "esp_system.h"
#include "esp_log.h"
static const char *TAG= "blod";
esp_ota_handle_t update_handle = 0 ;
const esp_partition_t *update_partition = NULL;
esp_err_t err;
void ota_start()
{
    update_partition = esp_ota_get_next_update_partition(NULL);
    assert(update_partition != NULL);
    ESP_LOGE(TAG, "Writing to partition subtype %d at offset 0x%x",
             update_partition->subtype, update_partition->address);
    err = esp_ota_begin(update_partition, OTA_WITH_SEQUENTIAL_WRITES, &update_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_ota_begin failed (%s)", esp_err_to_name(err));

        esp_ota_abort(update_handle);

    }else{
        ESP_LOGE(TAG, "StartFuckample");
    }
}


void ota_feed(char* data, int len)
{
    esp_ota_write( update_handle, (const void *)data, len);
}


void ota_end()
{
    esp_ota_end(update_handle);
    esp_ota_set_boot_partition(update_partition);
    esp_restart();
}