set(srcs)

idf_component_register(SRCS "myble.c" "bleworker.c"
                    INCLUDE_DIRS  "."
                    REQUIRES console nvs_flash bt ota
                    REQUIRED_IDF_TARGETS esp32)
