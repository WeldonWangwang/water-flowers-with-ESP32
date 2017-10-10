deps_config := \
	/home/wangwangwang/workspace/esp-idf/components/app_trace/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/aws_iot/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/bt/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/esp32/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/ethernet/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/fatfs/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/freertos/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/heap/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/log/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/lwip/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/mbedtls/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/openssl/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/pthread/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/spi_flash/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/spiffs/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/tcpip_adapter/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/wear_levelling/Kconfig \
	/home/wangwangwang/workspace/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/wangwangwang/workspace/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/wangwangwang/workspace/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/wangwangwang/workspace/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
