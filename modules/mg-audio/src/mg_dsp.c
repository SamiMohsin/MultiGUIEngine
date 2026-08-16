/**
 * @file mg_dsp.c
 * @brief MultiGUIEngine Digital Signal Processing (DSP) Audio Filter implementation.
 */
#include "mg/audio/dsp.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void mg_dsp_lowpass_init(mg_biquad_filter_t* filter, float cutoff_hz, float sample_rate, float q) {
    if (!filter || sample_rate <= 0.0f) return;
    if (q <= 0.0f) q = 0.7071f;

    float omega = 2.0f * (float)M_PI * cutoff_hz / sample_rate;
    float sn = sinf(omega);
    float cs = cosf(omega);
    float alpha = sn / (2.0f * q);

    float b0 = (1.0f - cs) * 0.5f;
    float b1 = 1.0f - cs;
    float b2 = (1.0f - cs) * 0.5f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cs;
    float a2 = 1.0f - alpha;

    filter->a0 = b0 / a0;
    filter->a1 = b1 / a0;
    filter->a2 = b2 / a0;
    filter->b1 = a1 / a0;
    filter->b2 = a2 / a0;
    filter->z1 = 0.0f;
    filter->z2 = 0.0f;
}

void mg_dsp_highpass_init(mg_biquad_filter_t* filter, float cutoff_hz, float sample_rate, float q) {
    if (!filter || sample_rate <= 0.0f) return;
    if (q <= 0.0f) q = 0.7071f;

    float omega = 2.0f * (float)M_PI * cutoff_hz / sample_rate;
    float sn = sinf(omega);
    float cs = cosf(omega);
    float alpha = sn / (2.0f * q);

    float b0 = (1.0f + cs) * 0.5f;
    float b1 = -(1.0f + cs);
    float b2 = (1.0f + cs) * 0.5f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cs;
    float a2 = 1.0f - alpha;

    filter->a0 = b0 / a0;
    filter->a1 = b1 / a0;
    filter->a2 = b2 / a0;
    filter->b1 = a1 / a0;
    filter->b2 = a2 / a0;
    filter->z1 = 0.0f;
    filter->z2 = 0.0f;
}

void mg_dsp_biquad_process(mg_biquad_filter_t* filter, float* samples, size_t sample_count) {
    if (!filter || !samples) return;
    for (size_t i = 0; i < sample_count; ++i) {
        float in = samples[i];
        float out = in * filter->a0 + filter->z1;
        filter->z1 = in * filter->a1 + filter->z2 - filter->b1 * out;
        filter->z2 = in * filter->a2 - filter->b2 * out;
        samples[i] = out;
    }
}

void mg_dsp_delay_apply(
    float* samples,
    size_t sample_count,
    float* delay_line,
    size_t delay_samples,
    float feedback,
    float mix
) {
    if (!samples || !delay_line || delay_samples == 0) return;
    for (size_t i = 0; i < sample_count; ++i) {
        size_t idx = i % delay_samples;
        float delayed = delay_line[idx];
        float input = samples[i];
        delay_line[idx] = input + delayed * feedback;
        samples[i] = input * (1.0f - mix) + delayed * mix;
    }
}
