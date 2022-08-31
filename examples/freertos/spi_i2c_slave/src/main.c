// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <platform.h>

#include "FreeRTOS.h"
#include "task.h"

#include "rtos_printf.h"

#include "platform/platform_init.h"
#include "platform/driver_instances.h"



void spi_master_task(void *arg) {
  rtos_printf("spi send 0x55\n");
  uint8_t spi_tx_buf[2]={0xAA,0x55};
  uint8_t spi_rx_buf[2];

  for (;;) {
    rtos_spi_master_delay_before_next_transfer(spi_device_ctx, 100);

    rtos_spi_master_transaction_start(spi_device_ctx);
    rtos_spi_master_transfer(spi_device_ctx, spi_tx_buf, spi_rx_buf, 2);
    rtos_spi_master_transaction_end(spi_device_ctx);

    // rtos_printf("SPI Tx spi_rx_buf[0]0x%x, spi_rx_buf[1]0x%x\n",spi_rx_buf[0],spi_rx_buf[1]);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void tile0_hello_task(void *arg) {
  rtos_printf("Hello task running from tile %d on core %d\n", THIS_XCORE_TILE,
              portGET_CORE_ID());
  
  for (;;) {
    rtos_printf("Hello from tile %d\n", THIS_XCORE_TILE);
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void create_hello_task()
{
#if ON_TILE(0)
  xTaskCreate((TaskFunction_t)tile0_hello_task, "tile0_hello_task",
              RTOS_THREAD_STACK_SIZE(tile0_hello_task), NULL,
              configMAX_PRIORITIES - 1, NULL);
#endif
}



void create_spi_master_task()
{
#if ON_TILE(0)
  xTaskCreate((TaskFunction_t)spi_master_task, "spi_master_task",
              RTOS_THREAD_STACK_SIZE(spi_master_task), NULL,
              configMAX_PRIORITIES - 1, NULL);
#endif
}

void startup_task(void *arg)
{
    rtos_printf("Startup task running from tile %d on core %d\n", THIS_XCORE_TILE, portGET_CORE_ID());

    platform_start();

#if ON_TILE(0)
    create_hello_task();
    create_spi_master_task();
#endif

	for (;;) {
		rtos_printf("Tile[%d]:\n\tMinimum heap free: %d\n\tCurrent heap free: %d\n", THIS_XCORE_TILE, xPortGetMinimumEverFreeHeapSize(), xPortGetFreeHeapSize());
		vTaskDelay(pdMS_TO_TICKS(5000));
	}
}

static void tile_common_init(chanend_t c)
{
    platform_init(c);
    chanend_free(c);

    xTaskCreate((TaskFunction_t) startup_task,
                "startup_task",
                RTOS_THREAD_STACK_SIZE(startup_task),
                NULL,
                10,
                NULL);

    rtos_printf("start scheduler on tile %d\n", THIS_XCORE_TILE);
    vTaskStartScheduler();
}

#if ON_TILE(0)
void main_tile0(chanend_t c0, chanend_t c1, chanend_t c2, chanend_t c3) {

  (void)c0;
  (void)c2;
  (void)c3;

  tile_common_init(c1);
}
#endif

#if ON_TILE(1)
void main_tile1(chanend_t c0, chanend_t c1, chanend_t c2, chanend_t c3) {
  (void)c1;
  (void)c2;
  (void)c3;

  tile_common_init(c0);
}
#endif