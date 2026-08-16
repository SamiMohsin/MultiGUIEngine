/**
 * @file main.c
 * @brief Example 11: Flexbox GUI Auto-Layout and Real-Time Audio DSP Filter Effects.
 */
#include "mg/ui/ui.h"
#include "mg/audio/audio.h"
#include "mg/render/render.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

int main(void) {
    printf("=== Example 11: Flexbox GUI Auto-Layout and Audio DSP Showcase ===\n");

    /* 1. Test Flexbox GUI Auto-Layout Engine */
    const float item_widths[4] = { 100.0f, 150.0f, 200.0f, 120.0f };
    const float item_heights[4] = { 40.0f, 40.0f, 40.0f, 40.0f };
    mg_flex_item_rect_t rects[4];

    mg_flex_config_t flex_row = {
        .direction = MG_FLEX_ROW,
        .justify = MG_JUSTIFY_START,
        .gap = 12.0f,
        .padding = 16.0f,
        .container_width = 800.0f,
        .container_height = 200.0f
    };
    mg_flex_compute(&flex_row, item_widths, item_heights, 4, 0.0f, 0.0f, rects);

    assert(rects[0].x == 16.0f);
    assert(rects[1].x == 16.0f + 100.0f + 12.0f);
    printf("PASS: Flexbox Row Auto-Layout computed (Item 0 x=%.1f, Item 1 x=%.1f)\n", rects[0].x, rects[1].x);

    /* Flexbox Wrap Mode */
    mg_flex_config_t flex_wrap = {
        .direction = MG_FLEX_WRAP,
        .justify = MG_JUSTIFY_START,
        .gap = 10.0f,
        .padding = 10.0f,
        .container_width = 300.0f, /* Forces wrap */
        .container_height = 400.0f
    };
    mg_flex_compute(&flex_wrap, item_widths, item_heights, 4, 0.0f, 0.0f, rects);
    assert(rects[2].y > rects[0].y); /* Item 2 wrapped to row 2 */
    printf("PASS: Flexbox Wrap Auto-Layout verified (Item 2 wrapped to y=%.1f)\n", rects[2].y);

    /* 2. Test Audio DSP Filters */
    float audio_buffer[512];
    for (int i = 0; i < 512; ++i) {
        audio_buffer[i] = sinf((float)i * 0.5f) + sinf((float)i * 2.0f) * 0.5f; /* Fundamental + high freq harmonic */
    }

    /* Apply Low-Pass Filter at 1000 Hz */
    mg_biquad_filter_t lpf;
    mg_dsp_lowpass_init(&lpf, 1000.0f, 44100.0f, 0.7071f);
    mg_dsp_biquad_process(&lpf, audio_buffer, 512);
    printf("PASS: Biquad Low-Pass Filter applied (Output sample 10: %.4f)\n", audio_buffer[10]);

    /* Apply Stereo Delay & Echo Effect */
    float delay_line[256] = { 0 };
    mg_dsp_delay_apply(audio_buffer, 512, delay_line, 256, 0.5f, 0.3f);
    printf("PASS: Stereo Delay / Echo Effect applied\n");

    printf("PASS: Example 11: Flexbox GUI Auto-Layout and Audio DSP completed successfully\n");
    return 0;
}
