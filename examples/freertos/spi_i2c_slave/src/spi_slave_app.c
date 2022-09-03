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



#if APPCONF_SPI_I2C_SLAVE_TEST == 1

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

    rtos_printf("spi SLAVE transaction\n");

    spi_slave_xfer_prepare(
            spi_slave_ctx,
            in_buf,
            SPI_TEST_BUF_SIZE,
            test_buf,
            SPI_TEST_BUF_SIZE);

    for (;;) {

        int ret = spi_slave_xfer_complete(
                spi_slave_ctx,
                (void**)&rx_buf,
                &rx_len,
                (void**)&tx_buf,
                &tx_len,
                portMAX_DELAY);
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
    rtos_printf("spi SLAVE start");
}

RTOS_SPI_SLAVE_CALLBACK_ATTR
static void spi_slave_xfer_done(rtos_spi_slave_t *ctx, void *app_data)
{
    rtos_printf("SPI SLAVE xfer done");

}



void start_spi_devices()
{
    rtos_printf("start_spi_devices\n");
    rtos_spi_slave_start(spi_slave_ctx,
                         (void *)&dummy_app_data,
                         spi_slave_start,
                         spi_slave_xfer_done,
                         SPI_SLAVE_ISR_CORE,
                         configMAX_PRIORITIES-1);

}

#endif //#if APPCONF_SPI_I2C_SLAVE_TEST == 1
