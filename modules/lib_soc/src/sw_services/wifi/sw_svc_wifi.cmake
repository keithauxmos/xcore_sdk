set(SW_SVC_NAME WIFI)
set(SW_SVC_ADD_COMPILER_FLAGS "")
set(SW_SVC_XC_SRCS
        "")
set(SW_SVC_C_SRCS
        "src/sw_services/wifi/afr_wifi/sl_wfx_iot_wifi.c"
        "src/sw_services/wifi/wifi_connection_mgr.c")
set(SW_SVC_ASM_SRCS
        "")
set(SW_SVC_INCLUDES
        "src/sw_services/wifi/api"
        "src/sw_services/wifi/afr_wifi"
        "src/sw_services/wifi/afr_wifi/types")
set(SW_SVC_DEPENDENT_MODULES
        "")
set(SW_SVC_OPTIONAL_HEADERS
        "aws_wifi_config.h")