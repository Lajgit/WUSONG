void JumpToApplication(void)
{
    typedef void (*AppEntry_t)(void);
    uint32_t stack;
    AppEntry_t entry;

    if (!AppIsValid())
        return;

    stack = *(volatile uint32_t *)APP_ADDR;
    entry = (AppEntry_t)(*(volatile uint32_t *)(APP_ADDR + 4U));
    __disable_irq();
    (void)HAL_UART_DeInit(&huart1);
    (void)HAL_DeInit();
    SysTick->CTRL = 0U;
    SysTick->LOAD = 0U;
    SysTick->VAL = 0U;
    SCB->VTOR = APP_ADDR;
    __set_MSP(stack);
    __DSB();
    __ISB();
    __enable_irq();
    entry();
}
