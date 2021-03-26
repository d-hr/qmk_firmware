#define IS_COMMAND() (get_mods() == MOD_MASK_CTRL)
#pragma once

// #define AUTO_SHIFT_TIMEOUT 150 // see https://beta.docs.qmk.fm/using-qmk/software-features/feature_auto_shift
// #define NO_AUTO_SHIFT_SPECIAL // do not autoshift special keys
#define TAPPING_TERM 175 // https://docs.qmk.fm/#/feature_tap_dance
#define SHORT_TAPPING_TERM 100 // https://docs.qmk.fm/#/feature_tap_dance
// #define TAP_CODE_DELAY 20 //https://beta.docs.qmk.fm/using-qmk/advanced-keycodes/feature_macros

#define COMBO_TERM 30
#define COMBO_COUNT 2

#ifdef AUDIO_ENABLE
    #define STARTUP_SONG SONG(PLANCK_SOUND)
    #define DEFAULT_LAYER_SONGS { SONG(QWERTY_SOUND), \
                                  SONG(WORKMAN_SOUND) \
                                }
    #define PITCH_STANDARD_A 440.0f
#endif
