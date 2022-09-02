// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <platform.h>

#include "FreeRTOS.h"
#include "task.h"

#include "rtos_printf.h"

#include "platform/platform_init.h"
#include "platform/driver_instances.h"

#include "platform/platform_init.h"
#include "platform/driver_instances.h"

#include "rtos_spi_slave.h"

#include "spi_slave_app.h"
#include "app_conf.h"

static uint8_t dummy_app_data=0;

static uint8_t test_buf[SPI_TEST_BUF_SIZE] = {0};
static uint32_t xfer_done_called = 0;

spi_test_ctx_t test_ctx;

void spi_slave_task(void *arg)
{
    uint8_t *rx_buf = NULL;
    size_t rx_len = 0;
    uint8_t *tx_buf = NULL;
    size_t tx_len = 0;

    uint8_t in_buf[SPI_TEST_BUF_SIZE] = {0};
    rtos_printf("spi_slave_task started running\n");

    rtos_printf("SLAVE transaction\n");

    spi_slave_xfer_prepare(
            spi_slave_ctx,
            in_buf,
            SPI_TEST_BUF_SIZE,
            test_buf,
            SPI_TEST_BUF_SIZE);

    for (;;) {

        rtos_printf("int ret\n");
        int ret = spi_slave_xfer_complete(
                spi_slave_ctx,
                (void**)&rx_buf,
                &rx_len,
                (void**)&tx_buf,
                &tx_len,
                portMAX_DELAY);
        rtos_printf("int ret finished\n");
        if (ret != 0)
        {
            rtos_printf("SLAVE failed. Transfer timed out");
        }

        if (rx_len != SPI_TEST_BUF_SIZE) {
            rtos_printf("SLAVE failed. RX len got %u expected %u", rx_len, SPI_TEST_BUF_SIZE);
        } else if (tx_len != SPI_TEST_BUF_SIZE) {
            rtos_printf("SLAVE failed. TX len got %u expected %u", tx_len, SPI_TEST_BUF_SIZE);
        }

        if (rx_buf == NULL) {
            rtos_printf("SLAVE failed. rx_buf is NULL");
        } else if (tx_buf == NULL) {
            rtos_printf("SLAVE failed. tx_buf is NULL");
        }

        for (int i=0; i<SPI_TEST_BUF_SIZE; i++)
        {
            if (rx_buf[i] != test_buf[i]) {
                rtos_printf("SLAVE failed. rx_buf[%d] got 0x%x expected 0x%x", i, rx_buf[i], test_buf[i]);
            } else if (tx_buf[i] != test_buf[i]) {
                rtos_printf("SLAVE failed. tx_buf[%d] got 0x%x expected 0x%x", i, tx_buf[i], test_buf[i]);
            }
        }

        if (xfer_done_called != 1) {
            rtos_printf("SLAVE failed. slave_xfer_done callback did not occur");
        }
    }
}
RTOS_SPI_SLAVE_CALLBACK_ATTR
static void spi_slave_start(rtos_spi_slave_t *ctx, void *app_data)
{
    (void) ctx;
    (void) app_data;
    rtos_printf("SLAVE start");
}

RTOS_SPI_SLAVE_CALLBACK_ATTR
static void spi_slave_xfer_done(rtos_spi_slave_t *ctx, void *app_data)
{
    // spi_test_ctx_t *test_ctx = (spi_test_ctx_t*)ctx->app_data;
    rtos_printf("SLAVE xfer done");
    // if (test_ctx->slave_xfer_done[test_ctx->cur_test] != NULL)
    // {
    //     SPI_SLAVE_XFER_DONE_ATTR spi_slave_xfer_done_t fn;
    //     fn = test_ctx->slave_xfer_done[test_ctx->cur_test];
    //     fn(ctx, app_data);
    // } else {
    //     rtos_printf("SLAVE missing slave_xfer_done callback on test %d", test_ctx->cur_test);
    // }
}

// static int run_spi_tests(spi_test_ctx_t *test_ctx, chanend_t c)
// {
//     int retval = 0;

//     do
//     {
//         sync(c);
//         if (test_ctx->main_test[test_ctx->cur_test] != NULL)
//         {
//             SPI_MAIN_TEST_ATTR spi_main_test_t fn;
//             fn = test_ctx->main_test[test_ctx->cur_test];
//             int tmp = fn(test_ctx);
//             retval = (retval != -1) ? tmp : retval;
//         } else {
//             rtos_printf("Missing main_test callback on test %d", test_ctx->cur_test);
//             retval = -1;
//         }
//     } while (++test_ctx->cur_test < test_ctx->test_cnt);

//     return retval;
// }

void start_spi_devices()
{
    rtos_printf("start_spi_devices\n");
    rtos_spi_slave_start(spi_slave_ctx,
                         &dummy_app_data,
                         spi_slave_start,
                         spi_slave_xfer_done,
                         SPI_SLAVE_ISR_CORE,
                         configMAX_PRIORITIES-1);

}

