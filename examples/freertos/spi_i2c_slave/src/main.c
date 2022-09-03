// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#include <platform.h>

#include "FreeRTOS.h"
#include "task.h"

#include "rtos_printf.h"

#include "platform/platform_init.h"
#include "platform/driver_instances.h"
#include "spi_slave_app.h"

#include "app_conf.h"



#if APPCONF_SPI_I2C_MASTER_TEST == 1
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

#define I2C_DEVICE_REG_ADD  0x45
void i2c_master_task(void *arg) {
  rtos_printf("i2c send 0x55\n");
  uint8_t I2C_tx_buf[2]={0x99,0x88};
  uint8_t I2C_rx_buf[5];
  size_t byte_sent=0;

  for (;;) {
    rtos_i2c_master_reg_write(i2c_master_ctx, I2C_SLAVE_ADDR, I2C_DEVICE_REG_ADD, I2C_tx_buf[0]);
    vTaskDelay(pdMS_TO_TICKS(500));

    rtos_i2c_master_reg_read(i2c_master_ctx, I2C_SLAVE_ADDR, I2C_DEVICE_REG_ADD, I2C_rx_buf);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void create_spi_master_task()
{
#if ON_TILE(0)
  xTaskCreate((TaskFunction_t)spi_master_task, "spi_master_task",
              RTOS_THREAD_STACK_SIZE(spi_master_task), NULL,
              configMAX_PRIORITIES - 1, NULL);
#endif
}

void create_i2c_master_task()
{
#if ON_TILE(0)
  xTaskCreate((TaskFunction_t)i2c_master_task, "i2c_master_task",
              RTOS_THREAD_STACK_SIZE(i2c_master_task), NULL,
              configMAX_PRIORITIES - 1, NULL);
#endif
}
#endif //#if APPCONF_SPI_I2C_MASTER_TEST == 1



#if APPCONF_SPI_I2C_SLAVE_TEST == 1

void create_spi_slave_task()
{
#if ON_TILE(0)
  xTaskCreate((TaskFunction_t)spi_slave_task, "spi_slave_task",
              RTOS_THREAD_STACK_SIZE(spi_slave_task), NULL,
              configMAX_PRIORITIES - 1, NULL);
#endif
}

void create_i2c_slave_task()
{
// #if ON_TILE(0)
//   xTaskCreate((TaskFunction_t)i2c_slave_task, "i2c_slave_task",
//               RTOS_THREAD_STACK_SIZE(i2c_slave_task), NULL,
//               configMAX_PRIORITIES - 1, NULL);
// #endif
}
#endif  //APPCONF_SPI_I2C_SLAVE_TEST

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


void startup_task(void *arg)
{
    rtos_printf("Startup task running from tile %d on core %d\n", THIS_XCORE_TILE, portGET_CORE_ID());

    platform_start();

#if ON_TILE(0)
    create_hello_task();

    #if APPCONF_SPI_I2C_MASTER_TEST == 1
    create_spi_master_task();
    create_i2c_master_task();
    #endif

    #if APPCONF_SPI_I2C_SLAVE_TEST == 1
    create_spi_slave_task();

    #endif

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