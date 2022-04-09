#include <stdio.h>
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"

#include "esp_event.h"
#include "freertos/semphr.h"
#include <sys/socket.h>

#include "wifi.h"
#include "storage.h"
#include "myble.h"
void app_main(void)
{
    initNvs();

    wifi_init_vaca();
    initBle();
}
