#include <stdio.h>
#include <strings.h>
#include <inttypes.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <nvs_flash.h>
#include <nvs.h>

static void print_value(const char *namespace, const char *key, nvs_type_t type) {
    nvs_handle_t nvs_handle;
    uint8_t *buf = malloc(4097);  // MAX NVS Value length = 4096
    size_t buf_len;
    esp_err_t err = nvs_open(namespace, NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        printf("ERROR: nvs_open(%s): %s\n", namespace, esp_err_to_name(err));
        return;
    }

    buf_len = 0;
    printf("namespace=%s key=%s ", namespace, key);
    switch (type) {
    case NVS_TYPE_U8: {
        uint8_t v;
        err = nvs_get_u8(nvs_handle, key, &v);
        printf("type=uint8(0x%x) value=%d\n", type, v);
        break;
    }
    case NVS_TYPE_I8: {
        int8_t v;
        err = nvs_get_i8(nvs_handle, key, &v);
        printf("type=int8(0x%x) value=%d\n", type, v);
        break;
    }
    case NVS_TYPE_U16: {
        uint16_t v;
        err = nvs_get_u16(nvs_handle, key, &v);
        printf("type=uint16(0x%x) value=%d\n", type, v);
        break;
    }
    case NVS_TYPE_I16: {
        int16_t v;
        err = nvs_get_i16(nvs_handle, key, &v);
        printf("type=int16(0x%x) value=%d\n", type, v);
        break;
    }
    case NVS_TYPE_U32: {
        uint32_t v;
        err = nvs_get_u32(nvs_handle, key, &v);
        printf("type=uint32(0x%x) value=%d\n", type, v);
        break;
    }
    case NVS_TYPE_I32: {
        int32_t v;
        err = nvs_get_i32(nvs_handle, key, &v);
        printf("type=int32(0x%x) value=%d\n", type, v);
        break;
    }
    case NVS_TYPE_U64: {
        uint64_t v;
        err = nvs_get_u64(nvs_handle, key, &v);
        printf("type=uint64(0x%x) value=%" PRIx64 "\n", type, v);
        break;
    }
    case NVS_TYPE_I64: {
        int64_t v;
        err = nvs_get_i64(nvs_handle, key, &v);
        printf("type=int64(0x%x) value=%" PRIi64 "\n", type, v);
        break;
    }
    case NVS_TYPE_STR:
        err = nvs_get_str(nvs_handle, key, (char *)buf, &buf_len);
        printf("type=str(0x%x) len=%d value=", type, buf_len);
        fwrite(buf, 1, buf_len, stdout);
        printf("\n");
        break;
    case NVS_TYPE_BLOB:
        err = nvs_get_blob(nvs_handle, key, buf, &buf_len);
        printf("type=blob(0x%x) value(hexdump)=", type);
        for (int i = 0; i < buf_len; i++) {
            printf("%02x", buf[i]);
        }
        printf("\n");
        break;
    default:
        printf("type=UNKNOWN(0x%x)\n", type);
        break;
    }

    nvs_close(nvs_handle);
    free(buf);
}

void app_main(void) {
    esp_err_t err;

    // Initialize NVS
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        printf("failed to nvs_flash_init(): %s\n", esp_err_to_name(err));
        goto fail;
    }
    ESP_ERROR_CHECK(err);

    printf("== NVS statistics ==\n");
    nvs_stats_t stats;
    err = nvs_get_stats("nvs", &stats);
    if (err != ESP_OK) {
        printf("failed to nvs_get_stats(): %s\n", esp_err_to_name(err));
        goto fail;
    }
    printf("NVS Entries: total=%d  used=%d  free=%d\n", stats.total_entries, stats.used_entries,
           stats.free_entries);
    printf("== done ==\n");

    printf("== Dump all NVS key-value pairs==\n");
    nvs_iterator_t it =
        nvs_entry_find("nvs", NULL, NVS_TYPE_ANY);  // "nvs" is the default NVS partition label
    while (it != NULL) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        print_value(info.namespace_name, info.key, info.type);
        it = nvs_entry_next(it);
    }
    nvs_release_iterator(it);

fail:
    printf("== done ==\n");
}