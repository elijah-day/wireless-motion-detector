idf_component_register(
	# Register component source files.
	SRCS "main.c" "motion_detection.c" "net.c" "wifi.c"

	# Register component include directories.
	INCLUDE_DIRS "."

	# Register component dependencies.
	REQUIRES "esp_adc" "esp_driver_gpio" "esp_wifi" "nvs_flash")
