/* Copyright 2015-2017 Jack Humbert
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
#include "muse.h"
// #include "keymap_common.h"
#include "keymap_extras/keymap_german.h"
#include "debug.h"
//#include "timer.h"  // Have to include timer for timer_read and timer_elapsed
#include "quantum.h"
#include "action.h"
#include "process_keycode/process_tap_dance.h"
#include "keymap_german.h"
// for german layout see: https://github.com/qmk/qmk_firmware/blob/master/quantum/keymap_extras/keymap_german.h
// for tapping (TD) see: https://docs.qmk.fm/#/feature_tap_dance , also https://thomasbaart.nl/2018/12/13/qmk-basics-tap-dance/ for idea on resetting
// thanks to: https://github.com/perkee/qmk_firmware/blob/space_cadet_every_mac_modifier/keyboards/iris/keymaps/perkee/keymap.c

enum planck_layers {
  _QWERTY,
  _SWAP6,
  _WORKMAN,
  _LOWER,
  _ANUM,
  _RAISE,
  _FKEY,
  _ADJUST,
};

enum planck_keycodes {
  QWERTY = SAFE_RANGE,
  SWAP6,
  WORKMAN,
  BACKLIT,
  EXT_PLV,
  KC_LSBO, // left shift bracket open (
  KC_RSBC, // right shift bracket close )
  KC_LCSO, // right control square bracket open [
  KC_RCSC, // right control square bracket close ]
  KC_LACO, // right alt curly bracket open {
  KC_RACC, // right alt curly bracket close }
};

#define LOWER _LOWER    // to lower
#define NUMP  _NUMP     // to nump
#define ANUM  _ANUM     // to anum
#define RAISE _RAISE    // to higher
#define FKEY  MO(_FKEY) // momentary to FKEY

typedef struct {
    bool is_press_action;
    int state;
} tap;

enum {
    HOLD = 1,
    SINGLE_TAP = 2,
    SPL_HE = 0,
    TD_UE,
    TD_OE,
    TD_SLBS,
    TD_DRAM,
    TD_DCOM,
    TD_EQEN,
    TD_SCLN,
    TD_PAST,
    TD_MDIV,
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
    [TD_EQEN] = ACTION_TAP_DANCE_DOUBLE(DE_EQL, KC_ENT),
    [TD_SCLN] = ACTION_TAP_DANCE_DOUBLE(DE_SCLN, DE_COLN),
    [TD_DCOM] = ACTION_TAP_DANCE_DOUBLE(DE_DOT, DE_COMM),
    [TD_PAST] = ACTION_TAP_DANCE_DOUBLE(DE_PLUS, DE_ASTR),
    [TD_MDIV] = ACTION_TAP_DANCE_DOUBLE(DE_MINS, DE_SLSH),
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

/* Qwerty --> shift (), ctrl [], alt {}; j+k is bksp, f+d is del
* ,-----------------------------------------------------------------------------------.
* |Tb/NUM|  Q@  |   W  |  E€  |   R  |   T  |   Z  |   U  |   I  |   O  |   #  |  ⌫   |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |Hy/Esc|   A  |   S  |   D  |   F  |   G  |   H  |   J  |   K  |   L  |   P  | Ent  |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |   ⇪  |   Y  |   X  |   C  |   V  |   B  |   N  |   M  |   ,  |   .  |   -  |  ⇪   |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* | Ctrl | FKEY | GUI  | Alt  |Lower |  SPB |  ENT |Raise | Alt  | /,\  | $,&  | Ctrl |
* `-----------------------------------------------------------------------------------'
*                             ^---------spacebar----------^
* Hyper is the linux hyper key (capslock in qmk)
*/
/* LOWER
* ,-----------------------------------------------------------------------------------.
* |  ^°  |   !  |   "  |   §  |   $  |   %  |   &  |  /,ü |   (  |  ),ö |   =  |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |      |  ä   |  ß ? |  <   |   >  |  '   |  `   |  XX  |  XX  |  XX  |   %  |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |      |  |   |  ~   |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |      |      |      |      |      |             |      |      |      |      |      |
* `-----------------------------------------------------------------------------------'
*/
/* Raise
* ,-----------------------------------------------------------------------------------.
* |      | ins  |  ⟸  |  ↑   |  ⟹  |   ⟰  |  ⟰   |  ⟸  |  ↑   |  ⟹  |  XX  |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* | Esc  |  XX  |   ←  |  ↓   |  →   |   ⟱  |  ⟱   |   ←  |  ↓   |  →   |  XX  | Del  |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |      |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |      |
* |------+------+------+------+------+------+------+------+------+------+------+------|
* |      |      |      |      |      |             |      |      |      |      |      |
* `-----------------------------------------------------------------------------------'
*/
/* Workman for German KB
 * ,-----------------------------------------------------------------------------------.
 * |Tb/NUM|   Q  |   D  |   R  |   W  |   B  |   J  |   F  |   U  |   P  |   #  |  ⌫   |
 * |------+------+------+------+------+-------------+------+------+------+------+------|
 * |Hy/Esc|   A  |   S  |   H  |   T  |   G  |   Y  |   N  |   E  |   O  |   I  | Ent  |
 * |------+------+------+------+------+------|------+------+------+------+------+------|
 * |   ⇪  |   Z  |   X  |   C  |   V  |   M  |   K  |   L  |   ,  |   .  |   -  |  ⇪   |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * | Ctrl | FKEY | GUI  | Alt  |Lower |  SPACEBAR   |Raise | Alt  | /,\  | $,&  | Ctrl |
 * `-----------------------------------------------------------------------------------'
 */
/* Adjust (Lower + Raise)
 *                      v------------------------RGB CONTROL--------------------v
 * ,-----------------------------------------------------------------------------------.
 * | Reset|  XX  |Debug | RGB  |RGBMOD| HUE+ | HUE- | SAT+ | SAT- |BRGTH+|BRGTH-|      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |Voice+|Aud TG|  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |  XX  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |Voice-|Mus TG|MU_MOD|CLK TG|CLK UP|CLK DN|CLK RS|Qwerty|WRKMAN|  XX  |      |
 * |------+------+------+------+------+------+------+------+------+------+------+------|
 * |      |      |      |      |      |             |      |      |      |      |      |
 * `-----------------------------------------------------------------------------------'
 */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[_QWERTY] = LAYOUT_planck_grid(
    KC_TAB,     KC_Q, KC_W,    KC_E,    KC_R,             KC_T,   KC_Y,            KC_U,             KC_I,    KC_O,        DE_HASH,     KC_BSPC,
    TD(SPL_HE), KC_A, KC_S,    KC_D,    KC_F,             KC_G,   KC_H,            KC_J,             KC_K,    KC_L,        KC_P,        KC_ENT,
    KC_LSBO,    KC_Z, KC_X,    KC_C,    KC_V,             KC_B,   KC_N,            KC_M,             DE_COMM, DE_DOT,      DE_MINS,     KC_RSBC,
    KC_LCSO,    FKEY, KC_LGUI, KC_C, LT(LOWER,KC_SPC), KC_SPC, LT(ANUM,KC_SPC), LT(RAISE,KC_SPC), KC_RACC, TD(TD_SLBS), TD(TD_DRAM), KC_RCSC
),
[_LOWER] = LAYOUT_planck_grid(
    DE_CIRC,  S(KC_1), S(KC_2), S(KC_3), S(KC_4), S(KC_5), S(KC_6),    TD(TD_UE), S(KC_8), TD(TD_OE), S(KC_0), _______,
    _______,  DE_ADIA, DE_SS,   DE_LABK, DE_RABK, DE_ACUT, S(DE_ACUT), DE_UDIA,   XXXXXXX, DE_ODIA,   S(KC_5), _______,
    _______,  DE_PIPE, DE_TILD, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX,   XXXXXXX, XXXXXXX,   XXXXXXX, _______,
    _______,  _______, _______, _______, _______, _______, _______,    _______,   _______, _______,   _______, _______
),
[_RAISE] = LAYOUT_planck_grid(
    _______, KC_INS,  KC_HOME, KC_UP,   KC_END,  KC_PGUP, KC_PGUP, KC_HOME, KC_UP,    KC_END,  XXXXXXX, _______,
    KC_ESC,  XXXXXXX, KC_LEFT, KC_DOWN, KC_RGHT, KC_PGDN, KC_PGDN, KC_LEFT, KC_DOWN,  KC_RGHT, XXXXXXX, _______,
    _______, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX, XXXXXXX, _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______, _______
),
[_ANUM] = LAYOUT_planck_grid(
    _______,  XXXXXXX, XXXXXXX, DE_SLSH, KC_5,    XXXXXXX, XXXXXXX,  KC_6,    DE_ASTR, XXXXXXX, XXXXXXX, _______,
    _______,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,     KC_7,    KC_8,    KC_9,    KC_0,    DE_EQL,
    _______,  XXXXXXX, XXXXXXX, XXXXXXX, DE_MINS, XXXXXXX, XXXXXXX,  DE_PLUS, _______, _______, _______, _______,
    _______,  _______, _______, _______, _______, _______, _______,  _______, _______, _______, _______, _______
),
[_FKEY] = LAYOUT_planck_grid(
    _______,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,    KC_F9,   KC_F10,  _______,
    _______,  KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,  KC_F16,  KC_F17,  KC_F18,   KC_F19,  KC_F20,  _______,
    _______,  XXXXXXX, XXXXXXX, XXXXXXX, _______, XXXXXXX, XXXXXXX, _______, XXXXXXX,  XXXXXXX, XXXXXXX, _______,
    _______,  _______, _______, _______, _______, _______, _______, _______, _______,  _______, _______, _______
),
[_SWAP6] = LAYOUT_planck_grid(
    KC_TAB,     KC_Q, KC_W,    KC_D,    KC_R,             KC_F,   KC_Y,            KC_U,             KC_K,    KC_L,        DE_HASH,     KC_BSPC,
    TD(SPL_HE), KC_A, KC_S,    KC_E,    KC_T,             KC_G,   KC_H,            KC_N,             KC_I,    KC_O,        KC_P,        KC_ENT,
    KC_LSBO,    KC_Z, KC_X,    KC_C,    KC_V,             KC_B,   KC_J,            KC_M,             DE_COMM, DE_DOT,      DE_MINS,     KC_RSBC,
    KC_LCSO,    FKEY, KC_LGUI, KC_LACO, LT(LOWER,KC_SPC), KC_SPC, LT(ANUM,KC_SPC), LT(RAISE,KC_SPC), KC_RACC, TD(TD_SLBS), TD(TD_DRAM), KC_RCSC
),
[_WORKMAN] = LAYOUT_planck_grid(
    KC_TAB,     KC_Q, KC_D,    KC_R,    KC_W,             KC_B,   KC_J,            KC_F,             KC_U,    KC_P,        DE_HASH,     KC_BSPC,
    TD(SPL_HE), KC_A, KC_S,    KC_H,    KC_T,             KC_G,   KC_Z,            KC_N,             KC_E,    KC_O,        KC_I,        KC_ENT,
    KC_LSBO,    KC_Y, KC_X,    KC_C,    KC_V,             KC_M,   KC_K,            KC_L,             DE_COMM, DE_DOT,      DE_MINS,     KC_RSBC,
    KC_LCSO,    FKEY, KC_LGUI, KC_LACO, LT(LOWER,KC_SPC), KC_SPC, LT(ANUM,KC_SPC), LT(RAISE,KC_SPC), KC_RACC, TD(TD_SLBS), TD(TD_DRAM), KC_RCSC
),
[_ADJUST] = LAYOUT_planck_grid(
    RESET,   XXXXXXX,  DEBUG,  RGB_TOG, RGB_MOD, RGB_HUI, RGB_HUD, RGB_SAI, RGB_SAD, RGB_VAI, RGB_VAD, _______,
    _______, MUV_IN,   AU_TOG, AU_ON,   AU_OFF,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, _______,
    _______, MUV_DE,   MU_TOG, MU_MOD,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  QWERTY, WORKMAN, SWAP6,   _______,
    _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
)
};

#ifdef AUDIO_ENABLE
  float plover_song[][2]     = SONG(PLOVER_SOUND);
  float plover_gb_song[][2]  = SONG(PLOVER_GOODBYE_SOUND);
#endif

layer_state_t layer_state_set_user(layer_state_t state) {
  return update_tri_layer_state(state, _LOWER, _RAISE, _ADJUST);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case QWERTY: {
      if (record->event.pressed) {
        print("QUERTZ\n");
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    }
    case WORKMAN: {
      if (record->event.pressed) {
        set_single_persistent_default_layer(_WORKMAN);
      }
      return false;
      break;
    }
    case SWAP6: {
        if (record->event.pressed) {
            set_single_persistent_default_layer(_SWAP6);
        }
        return false;
        break;
    }
    case BACKLIT: {
      if (record->event.pressed) {
        register_code(KC_RSFT);
        #ifdef BACKLIGHT_ENABLE
          backlight_step();
        #endif
        #ifdef KEYBOARD_planck_rev5
          writePinLow(E6);
        #endif
      } else {
        unregister_code(KC_RSFT);
        #ifdef KEYBOARD_planck_rev5
          writePinHigh(E6);
        #endif
      }
      return false;
      break;
    }
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

bool muse_mode = false;
uint8_t last_muse_note = 0;
uint16_t muse_counter = 0;
uint8_t muse_offset = 70;
uint16_t muse_tempo = 50;

void encoder_update(bool clockwise) {
  if (muse_mode) {
    if (IS_LAYER_ON(_RAISE)) {
      if (clockwise) {
        muse_offset++;
      } else {
        muse_offset--;
      }
    } else {
      if (clockwise) {
        muse_tempo+=1;
      } else {
        muse_tempo-=1;
      }
    }
  } else {
    if (clockwise) {
      #ifdef MOUSEKEY_ENABLE
        tap_code(KC_MS_WH_DOWN);
      #else
        tap_code(KC_PGDN);
      #endif
    } else {
      #ifdef MOUSEKEY_ENABLE
        tap_code(KC_MS_WH_UP);
      #else
        tap_code(KC_PGUP);
      #endif
    }
  }
}

void dip_switch_update_user(uint8_t index, bool active) {
    switch (index) {
        case 0: {
#ifdef AUDIO_ENABLE
            static bool play_sound = false;
#endif
            if (active) {
#ifdef AUDIO_ENABLE
                if (play_sound) { PLAY_SONG(plover_song); }
#endif
                layer_on(_ADJUST);
            } else {
#ifdef AUDIO_ENABLE
                if (play_sound) { PLAY_SONG(plover_gb_song); }
#endif
                layer_off(_ADJUST);
            }
#ifdef AUDIO_ENABLE
            play_sound = true;
#endif
            break;
        }
        case 1:
            if (active) {
                muse_mode = true;
            } else {
                muse_mode = false;
            }
    }
}

void matrix_scan_user(void) {
#ifdef AUDIO_ENABLE
    if (muse_mode) {
        if (muse_counter == 0) {
            uint8_t muse_note = muse_offset + SCALE[muse_clock_pulse()];
            if (muse_note != last_muse_note) {
                stop_note(compute_freq_for_midi_note(last_muse_note));
                play_note(compute_freq_for_midi_note(muse_note), 0xF);
                last_muse_note = muse_note;
            }
        }
        muse_counter = (muse_counter + 1) % muse_tempo;
    } else {
        if (muse_counter) {
            stop_all_notes();
            muse_counter = 0;
        }
    }
#endif
}

bool music_mask_user(uint16_t keycode) {
  switch (keycode) {
    case RAISE:
    case LOWER:
    case FKEY:
    case ANUM:
      return false;
    default:
      return true;
  }
}
