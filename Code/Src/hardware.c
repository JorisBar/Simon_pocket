#include "hardware.h"
#include "game.h"

typedef struct {
    GPIO_TypeDef *btn_port;
    uint8_t       btn_pin;
    GPIO_TypeDef *led_port;
    uint8_t       led_pin;
} simon_color_config_t;

static const simon_color_config_t SIMON_COLORS[COLOR_COUNT] = {
    [COLOR_RED]    = { GPIOA,  0,  GPIOA,  1 },
    [COLOR_GREEN]  = { GPIOB, 15,  GPIOA,  8 },
    [COLOR_BLUE]   = { GPIOB,  5,  GPIOB,  6 },
    [COLOR_YELLOW] = { GPIOA,  2,  GPIOA,  3 },
};


static uint8_t rtc_calculate_bcd_time(uint8_t delay_sec)
{
    uint8_t bcd = RTC->TR & (RTC_TR_ST_Msk | RTC_TR_SU_Msk);
    (void)RTC->DR;

    uint8_t seconds = (bcd >> 4) * 10 + (bcd & 0xF);
    seconds += delay_sec;

    if (seconds >= 60) {
        seconds -= 60;
    }
    return ((seconds / 10) << 4) | (seconds % 10);
}


// HARDWARE CONFIGURATION
void system_clock_config(void)
{
    RCC->CR |= RCC_CR_HSION;
    while (!(RCC->CR & RCC_CR_HSIRDY));
    RCC->CSR |= RCC_CSR_LSION;
    while (!(RCC->CSR & RCC_CSR_LSIRDY));

    RCC->CFGR |= RCC_CFGR_HPRE_DIV128;

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    PWR->CR |= PWR_CR_DBP;
    RCC->BDCR |= RCC_BDCR_RTCEN;
    RCC->BDCR |= RCC_BDCR_RTCSEL_LSI;
}

void gpio_init(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN |
                   RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOFEN;

    GPIOA->MODER = 0xEBFFFFFF; // Debugger alternate function
    GPIOB->MODER = 0xFFFFFFFF;
    GPIOC->MODER = 0xFFFFFFFF;
    GPIOF->MODER = 0xFFFFFFFF;

    // LED pins
    GPIOA->MODER &= ~(GPIO_MODER_MODER0 | GPIO_MODER_MODER2);
    GPIOB->MODER &= ~(GPIO_MODER_MODER5 | GPIO_MODER_MODER15);

    // Button pins
    GPIOA->MODER &= ~(GPIO_MODER_MODER1 | GPIO_MODER_MODER3 | GPIO_MODER_MODER8);
    GPIOB->MODER &= ~(GPIO_MODER_MODER6);
    GPIOA->MODER |= (GPIO_MODER_MODER1_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER8_0);
    GPIOB->MODER |= (GPIO_MODER_MODER6_0);

    RCC->AHBENR &= ~(RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIOFEN);

    led_toggle_all(false);
}

void exti_init(void)
{
    SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PA | SYSCFG_EXTICR1_EXTI2_PA;
    SYSCFG->EXTICR[1] |= SYSCFG_EXTICR2_EXTI5_PB;
    SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PB;
    EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR2 | EXTI_IMR_MR5 | EXTI_IMR_MR15;
    EXTI->FTSR |= EXTI_FTSR_TR0 | EXTI_FTSR_TR2 | EXTI_FTSR_TR5 | EXTI_FTSR_TR15;
    EXTI->PR = EXTI_IMR_MR0_Msk | EXTI_IMR_MR2_Msk | EXTI_IMR_MR5_Msk | EXTI_IMR_MR15_Msk;
    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_EnableIRQ(EXTI2_3_IRQn);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void rtc_init(void)
{
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;

    RTC->ISR &= ~RTC_ISR_ALRAF;
    RTC->CR &= ~RTC_CR_ALRAE;
    while (!(RTC->ISR & RTC_ISR_ALRAWF));
    RTC->ISR |= RTC_ISR_INIT;
    while (!(RTC->ISR & RTC_ISR_INITF));

    RTC->PRER = (0x7F << RTC_PRER_PREDIV_A_Pos);
    RTC->PRER |= RTC_PREDIV_SS;

    RTC->ISR &= ~RTC_ISR_INIT;
    RTC->ISR &= ~RTC_ISR_RSF;
    while (!(RTC->ISR & RTC_ISR_RSF));

    EXTI->PR = EXTI_PR_PR17;
    EXTI->IMR |= EXTI_IMR_MR17;
    EXTI->RTSR |= EXTI_RTSR_TR17;

    RTC->ALRMAR |= RTC_ALRMAR_MSK4_Msk | RTC_ALRMAR_MSK3_Msk | RTC_ALRMAR_MSK2_Msk;
    RTC->ALRMAR &= ~(RTC_ALRMAR_ST_Msk | RTC_ALRMAR_SU_Msk);
    RTC->CR |= RTC_CR_ALRAIE;
    EXTI->IMR |= EXTI_IMR_MR17_Msk;
    EXTI->RTSR |= EXTI_RTSR_TR17_Msk;
    NVIC_EnableIRQ(RTC_IRQn);
}

void adc_init(void)
{
    RCC->CR2 |= RCC_CR2_HSI14ON;
    while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    if ((ADC1->CR & ADC_CR_ADEN) != 0) {
        ADC1->CR |= ADC_CR_ADDIS;
        while ((ADC1->CR & ADC_CR_ADEN) != 0);
    }
    ADC1->CR |= ADC_CR_ADCAL;
    while ((ADC1->CR & ADC_CR_ADCAL) != 0);

    RCC->CR2 &= ~RCC_CR2_HSI14ON;
}


// --- HARDWARE ACTIONS ---
void adc_enable(void)
{
    RCC->CR2 |= RCC_CR2_HSI14ON;
    while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0);

    ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0);

    ADC1->CHSELR = ADC_CHSELR_CHSEL4;
}

void adc_disable(void)
{
    if (ADC1->CR & ADC_CR_ADSTART) {
        ADC1->CR |= ADC_CR_ADSTP;
        while (ADC1->CR & ADC_CR_ADSTP);
    }
    if (ADC1->CR & ADC_CR_ADEN) {
        ADC1->CR |= ADC_CR_ADDIS;
        while ((ADC1->CR & ADC_CR_ADEN) != 0);
    }
    RCC->CR2 &= ~RCC_CR2_HSI14ON;
}

uint8_t adc_generate_random_2bit(void)
{
    ADC1->CR |= ADC_CR_ADSTART;
    while ((ADC1->ISR & ADC_ISR_EOC) == 0);
    uint16_t raw_adc = ADC1->DR;
    raw_adc ^= raw_adc << 13;
    raw_adc ^= raw_adc >> 17;
    raw_adc ^= raw_adc << 5;
    return (raw_adc & 0x03);
}

void rtc_delay_ms(uint32_t delay_ms)
{
    RTC->ISR &= ~RTC_ISR_RSF;
    while (!(RTC->ISR & RTC_ISR_RSF));
    RTC->WPR = 0xCA;
    RTC->WPR = 0x53;
    RTC->CR &= ~RTC_CR_ALRAE;
    while (!(RTC->ISR & RTC_ISR_ALRAWF));

    uint32_t sub_seconds = RTC->SSR;
    uint32_t delay_seconds = 0;
    uint32_t delay_ticks = (uint32_t)delay_ms * (RTC_PREDIV_SS + 1) / 1000;

    delay_seconds = delay_ticks / (RTC_PREDIV_SS + 1);
    int32_t target = (int32_t)sub_seconds - (int32_t)(delay_ticks % (RTC_PREDIV_SS + 1));
    if (target < 0) {
        target += (RTC_PREDIV_SS + 1);
        delay_seconds++;
    }
    sub_seconds = (uint32_t)target;

    RTC->ALRMASSR = (0xFUL << 24) | sub_seconds;
    RTC->ALRMAR &= ~(RTC_ALRMAR_ST_Msk | RTC_ALRMAR_SU_Msk);
    RTC->ALRMAR |= rtc_calculate_bcd_time(delay_seconds);

    RTC->CR |= RTC_CR_ALRAE;
    RTC->WPR = 0xFE;
    RTC->WPR = 0x64;
}

void led_toggle_all(bool state)
{
    for (uint8_t i = 0; i < COLOR_COUNT; i++) {
        if (state) {
            led_on(i);
        } else {
            led_off(i);
        }
    }
}

void led_on(uint8_t color)
{
    SIMON_COLORS[color].led_port->ODR &= ~(1u << SIMON_COLORS[color].led_pin);
}

void led_off(uint8_t color)
{
    SIMON_COLORS[color].led_port->ODR |= (1u << SIMON_COLORS[color].led_pin);
}

bool led_is_on(uint8_t color)
{
    return (SIMON_COLORS[color].led_port->ODR & (1u << SIMON_COLORS[color].led_pin)) == 0;
}

bool button_is_pressed(uint8_t color)
{
    return (SIMON_COLORS[color].btn_port->IDR & (1u << SIMON_COLORS[color].btn_pin)) == 0;
}


// INTERRUPT HANDLERS
void EXTI0_1_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR0;
    exti_callback(COLOR_RED);
}

void EXTI2_3_IRQHandler(void)
{
    EXTI->PR = EXTI_PR_PR2;
    exti_callback(COLOR_YELLOW);
}

void EXTI4_15_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR5) {
        EXTI->PR = EXTI_PR_PR5;
        exti_callback(COLOR_BLUE);
    }
    if (EXTI->PR & EXTI_PR_PR15) {
        EXTI->PR = EXTI_PR_PR15;
        exti_callback(COLOR_GREEN);
    }
}

void RTC_IRQHandler(void)
{
    RTC->ISR &= ~RTC_ISR_ALRAF;
    EXTI->PR = (1 << 17);
    rtc_callback();
}
