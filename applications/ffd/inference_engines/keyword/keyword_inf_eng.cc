// Copyright (c) 2022 XMOS LIMITED. This Software is subject to the terms of the
// XMOS Public License: Version 1

/* STD headers */
#include <platform.h>
#include <xcore/hwtimer.h>
#include <xs1.h>

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "event_groups.h"
#include "stream_buffer.h"
#include "task.h"

/* Inference headers */
#include "InferenceEngine.hpp"
#include "tensorflow/lite/experimental/microfrontend/lib/frontend.h"
#include "tensorflow/lite/experimental/microfrontend/lib/frontend_util.h"

/* App headers */
#include "app_conf.h"
#include "keyword_inf_eng.h"
#include "platform/driver_instances.h"

#define NUM_FRAMES_PER_INFERENCE (2)
#define AUDIO_BUFFER_LENGTH                                                    \
  (appconfINFERENCE_FRAMES_PER_INFERENCE * NUM_FRAMES_PER_INFERENCE)
#define AUDIO_BUFFER_STRIDE_LENGTH                                             \
  (AUDIO_BUFFER_LENGTH - appconfINFERENCE_FRAMES_PER_INFERENCE)
#define AUDIO_WINDOW_LENGTH_MS                                                 \
  (((float)appconfINFERENCE_FRAMES_PER_INFERENCE /                             \
    (float)appconfAUDIO_PIPELINE_SAMPLE_RATE) *                                \
   1000 * NUM_FRAMES_PER_INFERENCE)

static void initialize_features(struct FrontendState *state) {
  struct FrontendConfig config;

  FrontendFillConfigWithDefaults(&config);
  config.window.size_ms = AUDIO_WINDOW_LENGTH_MS;
  config.window.step_size_ms = AUDIO_WINDOW_LENGTH_MS;
  config.filterbank.num_channels = 16;
  config.filterbank.lower_band_limit = 125.0;
  config.filterbank.upper_band_limit = 7500.0;
  config.noise_reduction.smoothing_bits = 10;
  config.noise_reduction.even_smoothing = 0.025;
  config.noise_reduction.odd_smoothing = 0.06;
  config.noise_reduction.min_signal_remaining = 0.05;
  config.pcan_gain_control.enable_pcan = false;
  config.log_scale.enable_log = true;
  config.log_scale.scale_shift = 6;

  FrontendPopulateState(&config, state, appconfAUDIO_PIPELINE_SAMPLE_RATE);
}

static void compute_features(struct FrontendOutput *output,
                             struct FrontendState *state, int16_t *audio16) {
  size_t num_samples_read;

  *output = FrontendProcessSamples(state, audio16, AUDIO_BUFFER_LENGTH,
                                   &num_samples_read);

  configASSERT(num_samples_read == AUDIO_BUFFER_LENGTH);
  for (int i = 0; i < output->size; ++i) {
    rtos_printf("%d ",
                (int)output->values[i]); // Print the feature
  }
  rtos_printf("\n");
}

void keyword_engine_task(keyword_engine_args_t *args) {
  StreamBufferHandle_t input_buf = args->samples_to_engine_stream_buf;
  EventGroupHandle_t output_egrp = args->egrp_inference;

  int32_t buf[appconfINFERENCE_FRAMES_PER_INFERENCE];

  struct FrontendState state;
  struct FrontendOutput frontend_output;
  int16_t audio16[AUDIO_BUFFER_LENGTH];
  size_t audio16_index = 0;
  int output_index = 0;

  /* Perform any initialization here */
  initialize_features(&state);

  // TODO:  store input tensor of features [65*16]
  //             Need to "quantize" the features
  // TODO: call inference every TBD audio frames

  while (1) {
    /* Receive audio frames */
    uint8_t *buf_ptr = (uint8_t *)buf;
    size_t buf_len = appconfINFERENCE_FRAMES_PER_INFERENCE * sizeof(int32_t);
    do {
      size_t bytes_rxed =
          xStreamBufferReceive(input_buf, buf_ptr, buf_len, portMAX_DELAY);
      buf_len -= bytes_rxed;
      buf_ptr += bytes_rxed;
    } while (buf_len > 0);

    for (int i = 0; i < appconfINFERENCE_FRAMES_PER_INFERENCE; ++i) {
      /* Audio is int32, convert to int16 */
      audio16[audio16_index++] = (int16_t)(buf[i] >> 16);
      if (audio16_index == AUDIO_BUFFER_LENGTH) {
        compute_features(&frontend_output, &state, audio16);

        /* Shift the audio buffer left one stride */
        memcpy(&audio16[0], &audio16[appconfINFERENCE_FRAMES_PER_INFERENCE],
               AUDIO_BUFFER_STRIDE_LENGTH * sizeof(audio16[0]));
        audio16_index = AUDIO_BUFFER_STRIDE_LENGTH;
      }
    }

    /* Perform inference here */
    // rtos_printf("inference\n");

    /* Set output event bits */
    // switch (output_index) {
    // default:
    // case 0:
    //   xEventGroupSetBits(output_egrp, INFERENCE_BIT_A | INFERENCE_BIT_B);
    //   break;
    // case 1:
    //   xEventGroupSetBits(output_egrp, INFERENCE_BIT_A);
    //   break;
    // case 2:
    //   xEventGroupSetBits(output_egrp, INFERENCE_BIT_B);
    //   break;
    // }
    // output_index = (output_index >= 2) ? 0 : output_index + 1;
  }
}