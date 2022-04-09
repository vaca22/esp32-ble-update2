#ifndef  H_MYWIFI_
#define  H_MYWIFI_
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include <string.h>
#include <sys/param.h>
#include<sys/_stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void wifi_init_vaca();

#ifdef __cplusplus
}
#endif

#endif