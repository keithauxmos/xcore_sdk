// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <platform.h>

#include "FreeRTOS.h"
#include "task.h"

#include "rtos_printf.h"

#include "platform/platform_init.h"
#include "platform/driver_instances.h"

#include "rtos_i2c_slave.h"

#include "i2c_slave_app.h"
#include "app_conf.h"

static uint8_t dummy_app_data_i2c=0;

i2c_test_ctx_t test_ctx_i2c;

#if APPCONF_SPI_I2C_SLAVE_TEST == 1

RTOS_I2C_SLAVE_CALLBACK_ATTR
static void i2c_slave_start(rtos_i2c_slave_t *ctx, void *app_data)
{
    rtos_printf("I2C SLAVE started");
}

RTOS_I2C_SLAVE_CALLBACK_ATTR
static void i2c_slave_rx(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len)
{
    int i;
    rtos_printf("i2c_slave_rx called, data len%d\n", len);
    for(i=0;i<len;i++){
        printf("i2c data %d: 0x%x \n",i,data[i]);
    }
}

RTOS_I2C_SLAVE_CALLBACK_ATTR
static size_t i2c_slave_tx_start(rtos_i2c_slave_t *ctx, void *app_data, uint8_t **data)
{
    size_t len = 5;
    uint8_t i2c_tx_data[5] = {1,2,3,4,5};

    rtos_printf("i2c_slave_tx_start\n");

    data = i2c_tx_data;
    return len;
}

RTOS_I2C_SLAVE_CALLBACK_ATTR
static void i2c_slave_tx_done(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len)
{
    rtos_printf("i2c_slave_tx_done called\n");
}

void start_i2c_slave()
{
    rtos_printf("start_i2c_slave\n");
    rtos_i2c_slave_start(i2c_slave_ctx,
                         &dummy_app_data_i2c,
                         i2c_slave_start,
                         i2c_slave_rx,
                         i2c_slave_tx_start,
                         i2c_slave_tx_done,
                         I2C_SLAVE_ISR_CORE,
                         configMAX_PRIORITIES-1);

}

#endif //APPCONF_SPI_I2C_SLAVE_TEST==1
