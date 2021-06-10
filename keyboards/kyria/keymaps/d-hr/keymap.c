/* Copyright 2019 Thomas Baart <thomas@splitkb.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H
#include "quantum.h"
#include "action.h"
#include "process_keycode/process_tap_dance.h"
#include "keymap_german.h"
#include "keymap_jp.h"

enum layers {
    _QWERTY = 0,
    _LOWER,
    _RAISE,
    _ANUM,
    _FKEY,
    _ADJUST
};

enum kyria_keycodes {
  KC_LSBO = SAFE_RANGE, // left shift bracket open (
  KC_RSBC, // right shift bracket close )
  KC_LCSO, // right control square bracket open [
  KC_RCSC, // right control square bracket close ]
  KC_LACO, // right alt curly bracket open {
  KC_RACC  // right alt curly bracket close }
};

#define LOWER _LOWER    // to lower
#define ANUM  _ANUM     // to anum
#define RAISE _RAISE    // to higher
#define FKEY  MO(_FKEY) // momentary to FKEY

/*
typedef struct {
    bool is_press_action;
    int state;
} tap;
*/

enum {
    HOLD = 1,
    SINGLE_TAP = 2,
    SPL_HE = 0,
    TD_UE,
    TD_OE,
    TD_SLBS,
    TD_DRAM,
    TD_ATEN,
};

static uint16_t space_cadet_timer = 0;

int cur_dance (qk_tap_dance_state_t *state) {
    if(state->pressed)
        return HOLD;

    if (state->count == 1)
        return SINGLE_TAP;

    return -1;
}

static int spl_hyes_tap_state = 0;

void spl_hyes_finished (qk_tap_dance_state_t *state, void *user_data) {
    spl_hyes_tap_state = cur_dance(state);
    switch (spl_hyes_tap_state) {
        case SINGLE_TAP:
            register_code(KC_ESC);
            break;
        case HOLD:
            register_code(KC_CAPS);
    }
}

void spl_hyes_reset (qk_tap_dance_state_t *state, void *user_data) {
    switch (spl_hyes_tap_state) {
        case SINGLE_TAP:
            unregister_code(KC_ESC);
            break;
        case HOLD:
            unregister_code(KC_CAPS);
            break;
    }
    spl_hyes_tap_state = 0;
}

// Tap Dance definitions
qk_tap_dance_action_t tap_dance_actions[] = {
    [TD_UE]   = ACTION_TAP_DANCE_DOUBLE(S(KC_7), DE_UDIA),
    [TD_OE]   = ACTION_TAP_DANCE_DOUBLE(S(KC_9), DE_ODIA),
    [TD_SLBS] = ACTION_TAP_DANCE_DOUBLE(DE_SLSH, DE_BSLS),
    [TD_DRAM] = ACTION_TAP_DANCE_DOUBLE(DE_DLR, DE_AMPR),
    [TD_ATEN] = ACTION_TAP_DANCE_DOUBLE(DE_AT, KC_APP),
    [SPL_HE]  = ACTION_TAP_DANCE_FN_ADVANCED(NULL,spl_hyes_finished, spl_hyes_reset),
};

enum combo_events {
  JK_BSP,
  FD_DEL,
};

const uint16_t PROGMEM bsp[] = {KC_J, KC_K, COMBO_END};
const uint16_t PROGMEM del[] = {KC_F, KC_D, COMBO_END};

combo_t key_combos[COMBO_COUNT] = {
    [JK_BSP] = COMBO(bsp,KC_BSPC),
    [FD_DEL] = COMBO(del,KC_DEL),
  };

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
/*
 * Base Layer: QWERTY
 *
 * ,-------------------------------------------.
 * | Tab    |   Q  |   W  |   E  |   R  |   T  |
 * |--------+------+------+------+------+------|
 * |Hyp/Esc |   A  |   S  |   D  |   F  |   G  |
 * |--------+------+------+------+------+------+-------------.
 * | Shft/( |   Z  |   X  |   C  |   V  |   B  | Fkey |  @   |
 * `----------------------+------+------+------+------+------|
 *                        | Super| Alt  | Lower| Space| Ctrl |
 *                        |      | {    | Space|      | [    |
 *                        `----------------------------------'
 *
 *                        ,-------------------------------------------.
 *                        |   Y  |   U  |   I  |   O  |   #  | Pipe   |
 *                        |------+------+------+------+------+--------|
 *                        |   H  |   J  |   K  |   L  |   P  | Sup/Ent|
 *          ,-------------+------+------+------+------+------+--------|
 *          | //\  |  $/& |   N  |   M  |   ,  |   .  |   -  | Shft/) |
 *          |------+------+------+------+------+----------------------'
 *          | Ctrl | Space| Space| AltGr|  XX  |
 *          |    ] | ANUM | Raise|     }|  XX  |
 *          `----------------------------------'
 */
    [_QWERTY] = LAYOUT_stack(
      KC_TAB,      KC_Q,   KC_W,    KC_E,    KC_R,     KC_T,
      TD(SPL_HE),  KC_A,   KC_S,    KC_D,    KC_F,     KC_G,
      KC_LSBO,     KC_Z,   KC_X,    KC_C,    KC_V,     KC_B,    FKEY,   TD(TD_ATEN),
                           KC_LGUI, KC_LACO, LT(LOWER, KC_SPC), KC_SPC, KC_LCSO,
//
                                        KC_Y,     KC_U,     KC_I,     KC_O,     DE_HASH, DE_PIPE,
                                        KC_H,     KC_J,     KC_K,     KC_L,     KC_P,    RGUI_T(KC_ENT),
       TD(TD_SLBS),  TD(TD_DRAM),       KC_N,     KC_M,     DE_COMM,  DE_DOT,   DE_MINS, KC_RSBC,
       KC_RCSC,      LT(ANUM, KC_SPC),  LT(RAISE, KC_SPC),  KC_RACC,  KC_F2
    ),
/*
 * Lower Layer: Symbols
 *'
 * ,-------------------------------------------.
 * |        |      |      |      |      |      |
 * |--------+------+------+------+------+------|
 * |        |      |      |      |      |      |
 * |--------+------+------+------+------+------+-------------.
 * |        |      |      |      |      |      |      |      |
 * `----------------------+------+------+------+------+------|
 *                        |      |      |      |      |      |
 *                        |      |      |      |      |      |
 *                        `----------------------------------'
 *
 *                        ,-------------------------------------------.
 *                        |      |      |      |      |      |        |
 *                        |------+------+------+------+------+--------|
 *                        |      |      |      |      |      |        |
 *          ,-------------+------+------+------+------+------+--------|
 *          |      |      |      |      |      |      |      |        |
 *          |------+------+------+------+------+----------------------'
 *          |      |      |      |      |      |
 *          |      |      |      |      |      |
 *          `----------------------------------'
 */
    [_LOWER] = LAYOUT_stack(
      DE_CIRC, S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5),
      _______, DE_ADIA, DE_SS,   DE_LABK, DE_RABK, DE_ACUT,
      _______, DE_PIPE, DE_TILD, XXXXXXX, XXXXXXX, XXXXXXX, _______, _______,
                                 _______, _______, _______, _______, _______,
//
                        S(KC_6),    TD(TD_UE), S(KC_8), TD(TD_OE), S(KC_0), _______,
                        S(DE_ACUT), DE_UDIA,   XXXXXXX, DE_ODIA,   S(KC_5), _______,
      _______, _______, XXXXXXX,    XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX, _______,
      _______, _______, _______,    _______,   _______
    ),
/*
 * Raise Layer: Number keys, media, navigation
 *
 * ,-------------------------------------------.
 * |        |      |      |      |      |      |
 * |--------+------+------+------+------+------|
 * |        |      |      |      |      |      |
 * |--------+------+------+------+------+------+-------------.
 * |        |      |      |      |      |      |      |      |
 * `----------------------+------+------+------+------+------|
 *                        |      |      |      |      |      |
 *                        |      |      |      |      |      |
 *                        `----------------------------------'
 *
 *                        ,-------------------------------------------.
 *                        |      |      |      |      |      |        |
 *                        |------+------+------+------+------+--------|
 *                        |      |      |      |      |      |        |
 *          ,-------------+------+------+------+------+------+--------|
 *          |      |      |      |      |      |      |      |        |
 *          |------+------+------+------+------+----------------------'
 *          |      |      |      |      |      |
 *          |      |      |      |      |      |
 *          `----------------------------------'
 */
    [_RAISE] = LAYOUT_stack(
      _______, XXXXXXX, DE_LPRN, XXXXXXX, DE_RPRN, XXXXXXX,
      _______, XXXXXXX, DE_LCBR, XXXXXXX, DE_RCBR, XXXXXXX,
      _______, XXXXXXX, DE_LBRC, XXXXXXX, DE_RBRC, XXXXXXX, _______, _______,
                                 _______, _______, _______, _______, _______,
//
                        KC_PGUP, KC_HOME, KC_UP,   KC_END,  XXXXXXX, _______,
                        KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, XXXXXXX, _______,
      _______, _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
      _______, _______, _______, _______, _______
    ),
    [_ANUM] = LAYOUT_stack(
      _______, XXXXXXX, XXXXXXX, DE_SLSH, KC_5,    XXXXXXX,
      _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,
      _______, XXXXXXX, XXXXXXX, XXXXXXX, DE_MINS, DE_CIRC,  _______, _______,
                                 _______, _______, _______,  _______, _______,
//
                        XXXXXXX,  KC_6,    DE_ASTR, XXXXXXX, XXXXXXX, _______,
                        KC_6,     KC_7,    KC_8,    KC_9,    KC_0,    DE_EQL,
      _______, _______, DE_CIRC,  DE_PLUS, _______, _______, _______, _______,
      _______, _______, _______, _______, _______
    ),
    [_FKEY] = LAYOUT_stack(
      _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,
      _______, KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,
      _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  _______, _______,
                                 _______, _______, _______,  _______, _______,
//
                        KC_F6,   KC_F7,   KC_F8,    KC_F9,   KC_F10,  _______,
                        KC_F16,  KC_F17,  KC_F18,   KC_F19,  KC_F20,  _______,
      _______, _______, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, _______,
      _______, _______, _______, _______, _______
    ),
/*
 * Adjust Layer: Function keys, RGB
 *
 * ,-------------------------------------------.
 * |        | F1   |  F2  | F3   | F4   | F5   |
 * |--------+------+------+------+------+------|
 * |        | TOG  | SAI  | HUI  | VAI  | MOD  |
 * |--------+------+------+------+------+------+-------------.
 * |        |      | SAD  | HUD  | VAD  | RMOD |      |      |
 * `----------------------+------+------+------+------+------|
 *                        |      |      |      |      |      |
 *                        |      |      |      |      |      |
 *                        `----------------------------------'
 *
 *                        ,-------------------------------------------.
 *                        | F6   | F7   |  F8  | F9   | F10  |        |
 *                        |------+------+------+------+------+--------|
 *                        |      |      |      | F11  | F12  |        |
 *          ,-------------+------+------+------+------+------+--------|
 *          |      |      |      |      |      |      |      |        |
 *          |------+------+------+------+------+----------------------'
 *          |      |      |      |      |      |
 *          |      |      |      |      |      |
 *          `----------------------------------'
 *
 */
    [_ADJUST] = LAYOUT_stack(
      _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,
      _______, RGB_TOG, RGB_SAI, RGB_HUI, RGB_VAI, RGB_MOD,
      _______, _______, RGB_SAD, RGB_HUD, RGB_VAD, RGB_RMOD,_______, _______,
                                 _______, _______, _______, _______, _______,
//
                        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,
                        _______, _______, _______, KC_F11,  KC_F12,  _______,
      _______, _______, _______, _______, _______, _______, _______, _______,
      _______, _______, _______, _______, _______

    ),
// /*
//  * Layer template
//  *
//  * ,-------------------------------------------.                              ,-------------------------------------------.
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------|                              |------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |                              |      |      |      |      |      |        |
//  * |--------+------+------+------+------+------+-------------.  ,-------------+------+------+------+------+------+--------|
//  * |        |      |      |      |      |      |      |      |  |      |      |      |      |      |      |      |        |
//  * `----------------------+------+------+------+------+------|  |------+------+------+------+------+----------------------'
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        |      |      |      |      |      |  |      |      |      |      |      |
//  *                        `----------------------------------'  `----------------------------------'
//  */
//     [_LAYERINDEX] = LAYOUT(
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______,                                     _______, _______, _______, _______, _______, _______,
//       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
//                                  _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
//     ),
};

layer_state_t layer_state_set_user(layer_state_t state) {
    return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    // shift ()
    case KC_LSBO: {
      if (record->event.pressed) {
        space_cadet_timer = timer_read();
        register_mods(
          MOD_BIT(KC_LSHIFT)
        );
      }
      else {
        if (timer_elapsed(space_cadet_timer) < SHORT_TAPPING_TERM) {
          register_code(DE_8);
          unregister_code(DE_8);
        }
        unregister_mods(MOD_BIT(KC_LSHIFT));
      }
      return false;
    }
    case KC_RSBC: {
      if (record->event.pressed) {
        space_cadet_timer = timer_read();
        register_mods(
          MOD_BIT(KC_RSHIFT)
        );
      }
      else {
        if (timer_elapsed(space_cadet_timer) < SHORT_TAPPING_TERM) {
          register_code(DE_9);
          unregister_code(DE_9);
        }
        unregister_mods(MOD_BIT(KC_RSHIFT));
      }
      return false;
    }
    // contol []
    case KC_LCSO: {
      if (record->event.pressed) {
        space_cadet_timer = timer_read();
        register_mods(
          MOD_BIT(KC_LCTRL)
        );
      }
      else {
        unregister_mods(
          MOD_BIT(KC_LCTRL)
        );
        if (timer_elapsed(space_cadet_timer) < SHORT_TAPPING_TERM) {
          register_mods(MOD_BIT(KC_RALT));
          register_code(DE_8);
          unregister_code(DE_8);
          unregister_mods(MOD_BIT(KC_RALT));
        }
      }
      return false;
    }
    case KC_RCSC: {
      if (record->event.pressed) {
        space_cadet_timer = timer_read();
        register_mods(
          MOD_BIT(KC_RCTRL)
        );
      }
      else {
        unregister_mods(
          MOD_BIT(KC_RCTRL)
        );
        if (timer_elapsed(space_cadet_timer) < SHORT_TAPPING_TERM) {
          register_mods(MOD_BIT(KC_RALT));
          register_code(DE_9);
          unregister_code(DE_9);
          unregister_mods(MOD_BIT(KC_RALT));
        }
      }
      return false;
    }
    // alt {}
    case KC_LACO: {
      if (record->event.pressed) {
        space_cadet_timer = timer_read();
        register_mods(
          MOD_BIT(KC_LALT)
        );
      }
      else {
        unregister_mods(
          MOD_BIT(KC_LALT)
        );
        if (timer_elapsed(space_cadet_timer) < SHORT_TAPPING_TERM) {
          register_mods(MOD_BIT(KC_RALT));
          register_code(DE_7);
          unregister_code(DE_7);
          unregister_mods(MOD_BIT(KC_RALT));
        }
      }
      return false;
    }
    case KC_RACC: {
      if (record->event.pressed) {
        space_cadet_timer = timer_read();
        register_mods(
          MOD_BIT(KC_RALT)
        );
      }
      else {
        if (timer_elapsed(space_cadet_timer) < SHORT_TAPPING_TERM) {
          register_code(DE_0);
          unregister_code(DE_0);
        }
        unregister_mods(MOD_BIT(KC_RALT));
      }
      return false;
  }
  }
  return true;
}


#ifdef OLED_DRIVER_ENABLE
oled_rotation_t oled_init_user(oled_rotation_t rotation) {
	return OLED_ROTATION_180;
}

static void render_kyria_logo(void) {
    static const char PROGMEM kyria_logo[] = {
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,128,128,192,224,240,112,120, 56, 60, 28, 30, 14, 14, 14,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7, 14, 14, 14, 30, 28, 60, 56,120,112,240,224,192,128,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,192,224,240,124, 62, 31, 15,  7,  3,  1,128,192,224,240,120, 56, 60, 28, 30, 14, 14,  7,  7,135,231,127, 31,255,255, 31,127,231,135,  7,  7, 14, 14, 30, 28, 60, 56,120,240,224,192,128,  1,  3,  7, 15, 31, 62,124,240,224,192,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,240,252,255, 31,  7,  1,  0,  0,192,240,252,254,255,247,243,177,176, 48, 48, 48, 48, 48, 48, 48,120,254,135,  1,  0,  0,255,255,  0,  0,  1,135,254,120, 48, 48, 48, 48, 48, 48, 48,176,177,243,247,255,254,252,240,192,  0,  0,  1,  7, 31,255,252,240,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,255,255,255,  0,  0,  0,  0,  0,254,255,255,  1,  1,  7, 30,120,225,129,131,131,134,134,140,140,152,152,177,183,254,248,224,255,255,224,248,254,183,177,152,152,140,140,134,134,131,131,129,225,120, 30,  7,  1,  1,255,255,254,  0,  0,  0,  0,  0,255,255,255,  0,  0,  0,  0,255,255,  0,  0,192,192, 48, 48,  0,  0,240,240,  0,  0,  0,  0,  0,  0,240,240,  0,  0,240,240,192,192, 48, 48, 48, 48,192,192,  0,  0, 48, 48,243,243,  0,  0,  0,  0,  0,  0, 48, 48, 48, 48, 48, 48,192,192,  0,  0,  0,  0,  0,
        0,  0,  0,255,255,255,  0,  0,  0,  0,  0,127,255,255,128,128,224,120, 30,135,129,193,193, 97, 97, 49, 49, 25, 25,141,237,127, 31,  7,255,255,  7, 31,127,237,141, 25, 25, 49, 49, 97, 97,193,193,129,135, 30,120,224,128,128,255,255,127,  0,  0,  0,  0,  0,255,255,255,  0,  0,  0,  0, 63, 63,  3,  3, 12, 12, 48, 48,  0,  0,  0,  0, 51, 51, 51, 51, 51, 51, 15, 15,  0,  0, 63, 63,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 48, 48, 63, 63, 48, 48,  0,  0, 12, 12, 51, 51, 51, 51, 51, 51, 63, 63,  0,  0,  0,  0,  0,
        0,  0,  0,  0, 15, 63,255,248,224,128,  0,  0,  3, 15, 63,127,255,239,207,141, 13, 12, 12, 12, 12, 12, 12, 12, 30,127,225,128,  0,  0,255,255,  0,  0,128,225,127, 30, 12, 12, 12, 12, 12, 12, 12, 13,141,207,239,255,127, 63, 15,  3,  0,  0,128,224,248,255, 63, 15,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  3,  7, 15, 62,124,248,240,224,192,128,  1,  3,  7, 15, 30, 28, 60, 56,120,112,112,224,224,225,231,254,248,255,255,248,254,231,225,224,224,112,112,120, 56, 60, 28, 30, 15,  7,  3,  1,128,192,224,240,248,124, 62, 15,  7,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
        0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  3,  7, 15, 14, 30, 28, 60, 56,120,112,112,112,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,224,112,112,112,120, 56, 60, 28, 30, 14, 15,  7,  3,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
    };
    oled_write_raw_P(kyria_logo, sizeof(kyria_logo));
}

static void render_qmk_logo(void) {
  static const char PROGMEM qmk_logo[] = {
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0};

  oled_write_P(qmk_logo, false);
}

static void render_status(void) {
    // QMK Logo and version information
    render_qmk_logo();
    oled_write_P(PSTR("Kyria rev1.4\n\n"), false);

    // Host Keyboard Layer Status
    oled_write_P(PSTR("Layer: "), false);
    switch (get_highest_layer(layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR("Default\n"), false);
            break;
        case _LOWER:
            oled_write_P(PSTR("Lower\n"), false);
            break;
        case _RAISE:
            oled_write_P(PSTR("Raise\n"), false);
            break;
        case _ADJUST:
            oled_write_P(PSTR("Adjust\n"), false);
            break;
        case _ANUM:
            oled_write_P(PSTR("Numbers\n"), false);
            break;
        case _FKEY:
            oled_write_P(PSTR("F-Key\n"), false);
            break;
        default:
            oled_write_P(PSTR("Undefined\n"), false);
    }

    // Host Keyboard LED Status
    uint8_t led_usb_state = host_keyboard_leds();
    oled_write_P(IS_LED_ON(led_usb_state, USB_LED_NUM_LOCK) ? PSTR("NUMLCK ") : PSTR("       "), false);
    oled_write_P(IS_LED_ON(led_usb_state, USB_LED_CAPS_LOCK) ? PSTR("CAPLCK ") : PSTR("       "), false);
    oled_write_P(IS_LED_ON(led_usb_state, USB_LED_SCROLL_LOCK) ? PSTR("SCRLCK ") : PSTR("       "), false);
}

void oled_task_user(void) {
    if (is_keyboard_master()) {
        render_status(); // Renders the current keyboard state (layer, lock, caps, scroll, etc)
    } else {
        render_kyria_logo();
    }
}
#endif

#ifdef ENCODER_ENABLE
bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        // Volume control
        if (clockwise) {
            tap_code(KC_VOLU);
        } else {
            tap_code(KC_VOLD);
        }
    }
    else if (index == 1) {
        // Page up/Page down
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    }
    return true;
}
#endif
