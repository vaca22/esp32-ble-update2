idf_component_register(SRCS "my_ota.c"
                    INCLUDE_DIRS  "."
                    PRIV_REQUIRES console app_update
                    REQUIRED_IDF_TARGETS esp32)
idf_component_get_property(mbedtls_lib mbedtls COMPONENT_LIB console)
target_link_libraries(${COMPONENT_LIB} PUBLIC ${mbedtls_lib})
