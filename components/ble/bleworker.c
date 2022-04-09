#include "bleworker.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include "myble.h"
#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "console/console.h"
#include "my_ota.h"

static const char *TAG = "BLEWORKER";
#define Buffer_Size 2000
#define CMD_Size 200
static unsigned char tcpReceive[Buffer_Size];
static int currentIndex = 0;
static unsigned char tcpcmd[CMD_Size];
static struct os_mbuf *om;

#define CMD_READ_FILE_DATA 0xF3

extern uint16_t conn_handle;
extern uint16_t hrs_hrm_handle;

static unsigned char crc8_compute(unsigned char *pdata, unsigned data_size,
                                  unsigned char crc_in) {
    uint8_t cnt;
    uint8_t crc_poly = 0x07;
    uint8_t data_tmp = 0;

    while (data_size--) {
        data_tmp = *(pdata++);
        crc_in ^= (data_tmp << 0);

        for (cnt = 0; cnt < 8; cnt++) {
            if (crc_in & 0x80) {
                crc_in = (crc_in << 1) ^ crc_poly;
            } else {
                crc_in = crc_in << 1;
            }
        }
    }

    return crc_in;
};

static void cmd_get() {
    unsigned int a = 0;
    unsigned int b = 0;
    switch (tcpcmd[1]) {
        case 0xf0:

            ESP_LOGI(TAG, "f0");

            break;

        case 0xf1:
            ESP_LOGI(TAG, "f1");
            break;
        case 0xf2:

            ESP_LOGI(TAG, "f2");

            break;

        case 0xf3:
            ESP_LOGI(TAG, "f3");
            break;


        case 0xf4:

            ESP_LOGI(TAG, "f4");

            break;

        case 0xf5:
            ESP_LOGI(TAG, "f5");
            break;

        default:
            break;
    }
};

static void handleDataPool() {
    int size = currentIndex + 1;
    int len;
    bool continueLoopFlag = false;
    while (1) {
        if (currentIndex < 5) {
            return;
        }
        size = currentIndex + 1;
        continueLoopFlag = false;
        for (int i = 0; i < size - 4; i++) {
            if (tcpReceive[i] == 0xa5 &&
                tcpReceive[i + 1] == ((unsigned char) (~tcpReceive[i + 2]))) {
                len = tcpReceive[i + 3];
                if (i + 5 + len <= size) {
                    for (int j = 0; j < len + 5; j++) {
                        tcpcmd[j] = tcpReceive[i + j];
                    }
                    if (crc8_compute(tcpcmd, len + 4, 0) == tcpcmd[len + 4]) {

                        if (i + 5 + len == size) {
                            currentIndex = 0;
                        } else {
                            for (int gg = i + 5 + len; gg < size; gg++) {
                                tcpReceive[gg - i - 5 - len] = tcpReceive[gg];
                            }
                            currentIndex = currentIndex - i - 5 - len;
                        }
                        cmd_get();
                        continueLoopFlag = true;
                    }
                }
            }
        }
        if (continueLoopFlag == false) {
            return;
        }
    }
};

void addBleWrite(unsigned char *thing, int len) {
    for (int k = 0; k < len; k++) {
        tcpReceive[currentIndex + k] = thing[k];
    }
    currentIndex += len;
    if (currentIndex > Buffer_Size) {
        currentIndex = 0;
    }
    handleDataPool();
}

int check(unsigned char *thing, int len) {
    unsigned char da=thing[0];
    unsigned char result=da;
    if(da==0){
        for(int k=1;k<len-1;k++){
            result=result^thing[k];
        }
        if(result==thing[len-1]){
            return 1;
        }
    }
    return 0;
}

int to3Int(unsigned char *thing){
    return thing[0]&(thing[1])<<8&(thing[2]<<16);
}
int total_len=0;
int total_len_index=0;
void update(unsigned char *thing, int len) {
    unsigned char mother[1]={0xaa};
    if(len>7){
        if(check(thing,len)){
            if(to3Int(thing)==0){
                total_len_index=0;
                total_len= to3Int(thing+3);
                ESP_LOGI(TAG, "good %d", total_len);
                ota_start();
            }
            total_len_index+=(len-7);
            ota_feed((char *)(thing+6),len-7);
            om = ble_hs_mbuf_from_flat(mother, 1);
            ble_gattc_notify_custom(conn_handle, hrs_hrm_handle, om);
            if(total_len_index==total_len){
                ota_end();
            }
        }
    }
}

static void sendFileData(unsigned char *contents, int len, unsigned char *mother, unsigned char cmd) {
    mother[0] = (unsigned char) 0xA5;
    mother[1] = (unsigned char) cmd;
    mother[2] = (unsigned char) ~cmd;
    mother[3] = len & 0xff;
    for (int k = 0; k < len; k++) {
        mother[4 + k] = contents[k];
    }
    mother[4 + len] = crc8_compute(mother, 4 + len, 0);
    om = ble_hs_mbuf_from_flat(mother, 5 + len);
    ble_gattc_notify_custom(conn_handle, hrs_hrm_handle, om);
};