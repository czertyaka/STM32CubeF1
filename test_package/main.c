#include "stm32f103xb.h"
#include <stdint.h>
#include <stddef.h>

void __libc_init_array(void) {}

#define ASSERT(X)       \
    if (!(X)) {     \
        *(int*)(0x00) = 0;    \
    }

int GPIO_init();
int SysTick_init();
int toggle_LED();
int update_SysTick_period(uint32_t msec);

int main(void) {
    ASSERT(GPIO_init() == 0);
    ASSERT(SysTick_init() == 0);
    while (1) {}
}

int GPIO_init() {
    // enable port C on APB2 bus
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    // configure PC13 as open-drain output with 10 MHz speed
    uint32_t CRH = GPIOC->CRH;
    CRH |= GPIO_CRH_CNF13_0;
    CRH &= ~GPIO_CRH_CNF13_1;
    CRH &= ~GPIO_CRH_MODE13_0;
    CRH |= GPIO_CRH_MODE13_1;
    GPIOC->CRH = CRH;
    return 0;
}

int SysTick_init() {
    SystemCoreClockUpdate();

    // enable systick exception, enable systick counter
    uint32_t systick_ctrl = SysTick->CTRL;
    systick_ctrl |= SysTick_CTRL_ENABLE_Msk;
    systick_ctrl |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL = systick_ctrl;

    // set reload value to 500 ms
    return update_SysTick_period(500);
}

int toggle_LED() {
    // toggle 13th bit of GPIOC_ODR (PC13)
    GPIOC->ODR ^= GPIO_ODR_ODR13;
    return 0;
}

int update_SysTick_period(uint32_t msec) {
    const uint32_t systick_freq = SystemCoreClock / 8; // Hz
    const uint32_t load = msec * systick_freq / 1000;
    if (load & ~SysTick_LOAD_RELOAD_Msk) {
        return -1;
    }
    SysTick->LOAD = load;
    return 0;
}

void SysTick_Handler(void) {
    ASSERT(toggle_LED() == 0);
    static const uint32_t periods[] = {
        400, 300, 200, 100, 200, 300, 400, 500
    };
    static size_t i = 0;
    ASSERT(i < sizeof(periods)/sizeof(periods[0]));
    ASSERT(update_SysTick_period(periods[i]) == 0);
    i = (i + 1) % (sizeof(periods)/sizeof(periods[0]));
}
