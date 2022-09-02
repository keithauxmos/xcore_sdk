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

RTOS_I2C_SLAVE_CALLBACK_ATTR
static void i2c_slave_start(rtos_i2c_slave_t *ctx, void *app_data)
{
    rtos_printf("I2C SLAVE started");
}

RTOS_I2C_SLAVE_CALLBACK_ATTR
static void i2c_slave_rx(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len)
{
    i2c_test_ctx_t *test_ctx_i2c = (i2c_test_ctx_t*)ctx->app_data;
    rtos_printf("i2c_slave_rx called\n");
    // if (test_ctx->slave_rx[test_ctx_i2c->cur_test] != NULL)
    // {
    //     I2C_SLAVE_RX_ATTR i2c_slave_rx_t fn;
    //     fn = test_ctx_i2c->slave_rx[test_ctx_i2c->cur_test];
    //     fn(ctx, app_data, data, len);
    // } else {
    //     rtos_printf("SLAVE missing slave_rx callback on test %d", test_ctx_i2c->cur_test);
    // }
}

RTOS_I2C_SLAVE_CALLBACK_ATTR
static size_t i2c_slave_tx_start(rtos_i2c_slave_t *ctx, void *app_data, uint8_t **data)
{
    i2c_test_ctx_t *test_ctx_i2c = (i2c_test_ctx_t*)ctx->app_data;
    size_t len = 0;

    // if (test_ctx_i2c->slave_tx_start[test_ctx_i2c->cur_test] != NULL)
    // {
    //     I2C_SLAVE_RX_ATTR i2c_slave_tx_start_t fn;
    //     fn = test_ctx_i2c->slave_tx_start[test_ctx_i2c->cur_test];
    //     len = fn(ctx, app_data, data);
    // } else {
    //     rtos_printf("SLAVE missing i2c_slave_tx_start callback on test %d", test_ctx_i2c->cur_test);
    // }

    return len;
}

RTOS_I2C_SLAVE_CALLBACK_ATTR
static void i2c_slave_tx_done(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len)
{
    i2c_test_ctx_t *test_ctx_i2c = (i2c_test_ctx_t*)ctx->app_data;
    // if (test_ctx_i2c->slave_tx_done[test_ctx_i2c->cur_test] != NULL)
    // {
    //     I2C_SLAVE_RX_ATTR i2c_slave_tx_done_t fn;
    //     fn = test_ctx_i2c->slave_tx_done[test_ctx_i2c->cur_test];
    //     fn(ctx, app_data, data, len);
    // } else {
    //     rtos_printf("SLAVE missing i2c_slave_tx_done callback on test %d", test_ctx_i2c->cur_test);
    // }
}

void start_i2c_slave()
{
    rtos_printf("rtos_spi_slave_start\n");
    rtos_i2c_slave_start(i2c_slave_ctx,
                         dummy_app_data_i2c,
                         i2c_slave_start,
                         i2c_slave_rx,
                         i2c_slave_tx_start,
                         i2c_slave_tx_done,
                         I2C_SLAVE_ISR_CORE,
                         configMAX_PRIORITIES-1);

}
