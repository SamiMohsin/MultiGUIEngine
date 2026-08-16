/**
 * @file dsp.h
 * @brief MultiGUIEngine Digital Signal Processing (DSP) Audio Filter Effects.
 */
#ifndef MG_AUDIO_DSP_H
#define MG_AUDIO_DSP_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mg_biquad_filter {
    float a0, a1, a2, b1, b2;
    float z1, z2;
} mg_biquad_filter_t;

/**
 * @brief Configures a biquad low-pass filter.
 */
void mg_dsp_lowpass_init(mg_biquad_filter_t* filter, float cutoff_hz, float sample_rate, float q);

/**
 * @brief Configures a biquad high-pass filter.
 */
void mg_dsp_highpass_init(mg_biquad_filter_t* filter, float cutoff_hz, float sample_rate, float q);

/**
 * @brief Processes an audio buffer with a biquad filter.
 */
void mg_dsp_biquad_process(mg_biquad_filter_t* filter, float* samples, size_t sample_count);

/**
 * @brief Applies delay and echo effect with feedback attenuation.
 */
void mg_dsp_delay_apply(
    float* samples,
    size_t sample_count,
    float* delay_line,
    size_t delay_samples,
    float feedback,
    float mix
);

#ifdef __cplusplus
}
#endif

#endif /* MG_AUDIO_DSP_H */
