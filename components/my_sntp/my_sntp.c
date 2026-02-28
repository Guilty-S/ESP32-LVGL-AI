#include <esp_sntp.h>
#include "time.h"
#include "custom.h"
void sntp_finish_callback(struct timeval *tv) {
    struct tm t;
    localtime_r(&tv->tv_sec, &t);
    set_home_time(&guider_ui, t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_wday, t.tm_hour, t.tm_min, t.tm_sec);
}

void my_sntp_init(void) {
    if (!esp_sntp_enabled()) {
        esp_sntp_setoperatingmode(SNTP_OPMODE_POLL);
        esp_sntp_setservername(0, "ntp.aliyun.com");
        esp_sntp_setservername(1, "time.asia.apple.com");
        esp_sntp_setservername(2, "pool.ntp.org");
        esp_sntp_set_time_sync_notification_cb(sntp_finish_callback);
        esp_sntp_init();
    }
}