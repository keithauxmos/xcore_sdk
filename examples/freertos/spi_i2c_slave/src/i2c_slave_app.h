// Copyright 2021 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#ifndef I2C_SLAVE_APP_H_
#define I2C_SLAVE_APP_H_

// #define SPI_MAX_TESTS   4

// #define SPI_MAIN_TEST_ATTR          __attribute__((fptrgroup("rtos_test_spi_main_test_fptr_grp")))
// #define SPI_SLAVE_XFER_DONE_ATTR    __attribute__((fptrgroup("rtos_test_spi_slave_xfer_done_fptr_grp")))

typedef struct i2c_test_ctx i2c_test_ctx_t;

struct i2c_test_ctx {
    // uint32_t cur_test;
    // uint32_t test_cnt;
    // char *name[I2C_MAX_TESTS];

    rtos_i2c_slave_t *i2c_slave_ctx;
    // int slave_success[I2C_MAX_TESTS];

    // I2C_MAIN_TEST_ATTR int (*main_test[I2C_MAX_TESTS])(i2c_test_ctx_t *ctx);
    // I2C_SLAVE_RX_ATTR void (*slave_rx[I2C_MAX_TESTS])(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len);
    // I2C_SLAVE_TX_START_ATTR size_t (*slave_tx_start[I2C_MAX_TESTS])(rtos_i2c_slave_t *ctx, void *app_data, uint8_t **data);
    // I2C_SLAVE_TX_DONE_ATTR void (*slave_tx_done[I2C_MAX_TESTS])(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len);
};

typedef int (*i2c_main_test_t)(i2c_test_ctx_t *ctx);
typedef void (*i2c_slave_rx_t)(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len);
typedef size_t (*i2c_slave_tx_start_t)(rtos_i2c_slave_t *ctx, void *app_data, uint8_t **data);
typedef void (*i2c_slave_tx_done_t)(rtos_i2c_slave_t *ctx, void *app_data, uint8_t *data, size_t len);

// /* Local Tests */
// void register_single_transaction_test(spi_test_ctx_t *test_ctx);
// void register_multiple_transaction_test(spi_test_ctx_t *test_ctx);

// /* RPC Tests */
// void register_rpc_single_transaction_test(spi_test_ctx_t *test_ctx);
// void register_rpc_multiple_transaction_test(spi_test_ctx_t *test_ctx);

void i2c_slave_task(void *arg);

void start_i2c_slave();

#endif /* I2C_SLAVE_APP_H_ */
