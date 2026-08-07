#include "hardware.h"
#include "game.h"


typedef enum {
    SYS_STATE_STOP,
    SYS_STATE_START,
    SYS_STATE_PLAY,
    SYS_STATE_WAIT,
    SYS_STATE_WRONG,
    SYS_STATE_SCORE
} system_state_t;

typedef enum {
    RTC_STATE_SHORT_PRESS,
    RTC_STATE_LONG_PRESS,
    RTC_STATE_TIMEOUT
} rtc_state_t;


static volatile system_state_t g_system_state = SYS_STATE_STOP;
static volatile rtc_state_t    g_rtc_state;

static uint8_t                 g_answer_sequence[100];
static volatile uint8_t        g_score = 0;
static volatile uint8_t        g_current_round = 0;



void game_generate_new_sequence(void)
{
    adc_enable();
    for (uint8_t i = 0; i < 100; i++) {
        g_answer_sequence[i] = adc_generate_random_2bit();
    }

    adc_disable();
}

void exti_callback(uint8_t color)
{
    system_clock_config();
    g_rtc_state = RTC_STATE_SHORT_PRESS;

    switch (g_system_state) {
    case SYS_STATE_STOP:
        g_rtc_state = RTC_STATE_LONG_PRESS;
        rtc_delay_ms(3000);
        break;

    case SYS_STATE_START:
        led_toggle_all(true);
        rtc_delay_ms(300);
        break;

    case SYS_STATE_WAIT:
        if (color != g_answer_sequence[g_current_round]) {
            g_system_state = SYS_STATE_WRONG;
            led_toggle_all(true);
            rtc_delay_ms(1000);
        } else {
            led_on(color);
            rtc_delay_ms(200);
        }
        break;

    case SYS_STATE_SCORE:
        g_system_state = SYS_STATE_START;
        g_current_round = 0;
        g_score = 0;
        g_rtc_state = RTC_STATE_TIMEOUT;
        rtc_delay_ms(30000);
        break;

    default:
        break;
    }
}

void rtc_callback(void)
{
    system_clock_config();

    if (g_rtc_state == RTC_STATE_TIMEOUT) {
        led_toggle_all(false);
        g_system_state = SYS_STATE_STOP;
    }

    if (g_rtc_state == RTC_STATE_LONG_PRESS) {
        if ((button_is_pressed(COLOR_RED) || button_is_pressed(COLOR_YELLOW)) ||
            (button_is_pressed(COLOR_BLUE) || button_is_pressed(COLOR_GREEN))) {
            if (g_system_state == SYS_STATE_STOP) {
                g_score = 0;
                g_current_round = 0;
                game_generate_new_sequence();
                g_system_state = SYS_STATE_START;
                led_toggle_all(true);
                g_rtc_state = RTC_STATE_TIMEOUT;
                rtc_delay_ms(30000);
            } else {
                led_toggle_all(false);
                g_system_state = SYS_STATE_STOP;
            }
        }
        return;
    }

    switch (g_system_state) {
    case SYS_STATE_WAIT:
        led_toggle_all(false);
        if (g_current_round == g_score) {
            g_system_state = SYS_STATE_PLAY;
            g_score++;
            g_current_round = 0;
            rtc_delay_ms(1000);
        } else {
            g_current_round++;
            g_rtc_state = RTC_STATE_LONG_PRESS;
            rtc_delay_ms(3000);
        }
        break;

    case SYS_STATE_PLAY:
        if (g_current_round <= g_score) {
            if (led_is_on(g_answer_sequence[g_current_round])) {
                led_off(g_answer_sequence[g_current_round]);
                g_current_round++;
                rtc_delay_ms(500); // Gap between notes
            } else {
                led_on(g_answer_sequence[g_current_round]);
                rtc_delay_ms(200); // Length of the note
            }
        } else {
            g_current_round = 0;
            g_system_state = SYS_STATE_WAIT;
            g_rtc_state = RTC_STATE_LONG_PRESS;
            rtc_delay_ms(1700);
        }
        break;

    case SYS_STATE_START:
        led_toggle_all(false);
        g_system_state = SYS_STATE_PLAY;
        rtc_delay_ms(1000);
        break;

    case SYS_STATE_WRONG:
        led_toggle_all(false);
        g_current_round = 0;
        g_system_state = SYS_STATE_SCORE;
        rtc_delay_ms(500);
        break;

    case SYS_STATE_SCORE:
        if (g_current_round < g_score) {
            uint8_t current_color = g_answer_sequence[g_current_round];

            if (led_is_on(current_color)) {
                led_off(current_color);
                g_current_round++;
                rtc_delay_ms(500); // Gap between notes
            } else {
                led_on(current_color);
                rtc_delay_ms(200); // Length of the note
            }
        } else {
            g_score = 0;
            g_current_round = 0;
            game_generate_new_sequence();
            g_system_state = SYS_STATE_START;
            led_toggle_all(true);
            g_rtc_state = RTC_STATE_TIMEOUT;
            rtc_delay_ms(30000);
        }
        break;

    default:
        break;
    }
}
