#include "hardware.h"
#include "stm32f030x6.h"
#include "game.h"

int main(void)
{
    system_clock_config();
    gpio_init();
    adc_init();
    rtc_init();
    exti_init();

    while (1)
    {
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        PWR->CR &= ~PWR_CR_PDDS;
        PWR->CR |= PWR_CR_LPDS;

        __WFI();
    }
}
