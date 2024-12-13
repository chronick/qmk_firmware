// Copyright 2024 jack (@waffle87)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "lib/oled.h"
#include "timer.h"
// #include "rgblight.h"

// Declare a flag and timer variable
bool show_emoji = false;
uint16_t emoji_timer = 0;

#ifdef OLED_ENABLE
oled_rotation_t oled_init_kb(oled_rotation_t rotation) {
    if (!is_keyboard_master()) {
        return OLED_ROTATION_180;
    }
    return rotation;
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
        return false;
    }
    if (is_keyboard_master()) {
        render_layer_state();
    } else {
        // This assumes you have bs_logo_img defined somewhere else as in your original code
        oled_write_raw_P(bs_logo_img, sizeof(bs_logo_img));
    }
    return false;
}

void oled_render_boot(bool bootloader) {
    oled_clear();
    for (int i = 0; i < 16; i++) {
        oled_set_cursor(0, i);
        if (bootloader) {
            oled_write_P(PSTR("Awaiting New Firmware "), false);
        } else {
            oled_write_P(PSTR("Rebooting "), false);
        }
    }

    oled_render_dirty(true);
}

bool shutdown_user(bool jump_to_bootloader) {
    oled_render_boot(jump_to_bootloader);
    return false;
}

#endif // OLED_ENABLE

void keyboard_post_init_user(void) {
    // Initialize RGB
    rgblight_enable_noeeprom();            // enables RGB without saving state
    rgblight_sethsv_noeeprom(180, 255, 255); // sets a default color (teal/cyan)
    rgblight_mode_noeeprom(3);            // sets a mode (e.g., breathing)

#ifdef OLED_ENABLE
    // If you want to show the logo at startup on slave half
    oled_write_raw_P(bs_logo_img, sizeof(bs_logo_img));
#endif
}

// This function is called whenever the layer state changes.
layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);

    // Set the flag and start the timer
    show_emoji = true;
    emoji_timer = timer_read();

    // Update the RGB lighting based on the active layer
    switch (layer) {
        case 0: // Base Layer (Colemak-DH)
            rgblight_sethsv_noeeprom(180, 255, 255); // Teal
            break;
        case 1: // Symbols/Numbers
            rgblight_sethsv_noeeprom(0, 255, 255);   // Red
            break;
        case 2: // RGB/Reset
            rgblight_sethsv_noeeprom(43, 255, 255);  // Yellow
            break;
        case 3: // QWERTY
            rgblight_sethsv_noeeprom(85, 255, 255);  // Green
            break;
        case 4: // Gaming or another layer
            rgblight_sethsv_noeeprom(128, 255, 255); // Blue
            break;
        default:
            rgblight_sethsv_noeeprom(180, 255, 255); // Default Teal for unknown layers
            break;
    }

    return state;
}

