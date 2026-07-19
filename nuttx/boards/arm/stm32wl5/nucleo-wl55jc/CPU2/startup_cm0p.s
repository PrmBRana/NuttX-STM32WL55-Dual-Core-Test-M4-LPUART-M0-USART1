/****************************************************************************
 * boards/arm/stm32wl5/nucleo-wl55jc/cpu2/startup_cm0p.s
 *
 * Cortex-M0+ startup: vector table, Reset_Handler, weak defaults
 ****************************************************************************/

  .syntax unified
  .cpu cortex-m0plus
  .thumb

  .global Reset_Handler
  .global Default_Handler
  .global IPCC_C2_RX_IRQHandler

/* ── Vector table ─────────────────────────────────────────────────────── */
  .section .isr_vector,"a",%progbits
  .type g_vectors, %object

g_vectors:
  /* Core vectors */
  .word _estack                   /* Stack top                        */
  .word Reset_Handler             /* Reset                            */
  .word NMI_Handler               /* NMI                              */
  .word HardFault_Handler         /* HardFault                        */
  .word 0                         /* reserved                         */
  .word 0                         /* reserved                         */
  .word 0                         /* reserved                         */
  .word 0                         /* reserved                         */
  .word 0                         /* reserved                         */
  .word 0                         /* reserved                         */
  .word 0                         /* reserved                         */
  .word SVC_Handler               /* SVCall                           */
  .word 0                         /* reserved                         */
  .word 0                         /* reserved                         */
  .word PendSV_Handler            /* PendSV                           */
  .word SysTick_Handler           /* SysTick                          */
  /* External interrupts (IRQ0..IRQ63) */
  .word Default_Handler           /* IRQ0  WWDG                       */
  .word IPCC_C2_RX_IRQHandler     /* IRQ1  IPCC C2 RX occupied ← KEY  */
  .word Default_Handler           /* IRQ2  IPCC C2 TX free            */
  .word Default_Handler           /* IRQ3  HSEM                       */
  .word Default_Handler           /* IRQ4  FLASH                      */
  .word Default_Handler           /* IRQ5  RCC                        */
  .word Default_Handler           /* IRQ6  EXTI0                      */
  .word Default_Handler           /* IRQ7  EXTI1                      */
  .word Default_Handler           /* IRQ8  EXTI2                      */
  .word Default_Handler           /* IRQ9  EXTI3                      */
  .word Default_Handler           /* IRQ10 EXTI4                      */
  .word Default_Handler           /* IRQ11 DMA1_CH1                   */
  .word Default_Handler           /* IRQ12 DMA1_CH2                   */
  .word Default_Handler           /* IRQ13 DMA1_CH3                   */
  .word Default_Handler           /* IRQ14 DMA1_CH4                   */
  .word Default_Handler           /* IRQ15 DMA1_CH5                   */
  .word Default_Handler           /* IRQ16 DMA1_CH6                   */
  .word Default_Handler           /* IRQ17 DMA1_CH7                   */
  .word Default_Handler           /* IRQ18 ADC                        */
  .word Default_Handler           /* IRQ19 DAC                        */
  .word Default_Handler           /* IRQ20 C2SEV                      */
  .word Default_Handler           /* IRQ21 COMP                       */
  .word Default_Handler           /* IRQ22 EXTI5..9                   */
  .word Default_Handler           /* IRQ23 TIM1_BRK                   */
  .word Default_Handler           /* IRQ24 TIM1_UP                    */
  .word Default_Handler           /* IRQ25 TIM1_TRG                   */
  .word Default_Handler           /* IRQ26 TIM1_CC                    */
  .word Default_Handler           /* IRQ27 TIM2                       */
  .word Default_Handler           /* IRQ28 TIM16                      */
  .word Default_Handler           /* IRQ29 TIM17                      */
  .word Default_Handler           /* IRQ30 I2C1_EV                    */
  .word Default_Handler           /* IRQ31 I2C1_ER                    */
  .word Default_Handler           /* IRQ32 I2C2_EV                    */
  .word Default_Handler           /* IRQ33 I2C2_ER                    */
  .word Default_Handler           /* IRQ34 SPI1                       */
  .word Default_Handler           /* IRQ35 SPI2                       */
  .word Default_Handler           /* IRQ36 USART1                     */
  .word Default_Handler           /* IRQ37 USART2                     */
  .word Default_Handler           /* IRQ38 LPUART1                    */
  .word Default_Handler           /* IRQ39 LPTIM1                     */
  .word Default_Handler           /* IRQ40 LPTIM2                     */
  .word Default_Handler           /* IRQ41 EXTI10..15                 */
  .word Default_Handler           /* IRQ42 RTC_ALARM                  */
  .word Default_Handler           /* IRQ43 LPTIM3                     */
  .word Default_Handler           /* IRQ44 SUBGHZSPI                  */
  .word Default_Handler           /* IRQ45 IPCC_C2_TX free            */
  .word Default_Handler           /* IRQ46 HSEM                       */
  .word Default_Handler           /* IRQ47 I2C3_EV                    */
  .word Default_Handler           /* IRQ48 I2C3_ER                    */
  .word Default_Handler           /* IRQ49 SUBGHZ_RADIO               */
  .word Default_Handler           /* IRQ50 AES                        */
  .word Default_Handler           /* IRQ51 RNG                        */
  .word Default_Handler           /* IRQ52 PKA                        */
  .word Default_Handler           /* IRQ53 DMA2_CH1                   */
  .word Default_Handler           /* IRQ54 DMA2_CH2                   */
  .word Default_Handler           /* IRQ55 DMA2_CH3                   */
  .word Default_Handler           /* IRQ56 DMA2_CH4                   */
  .word Default_Handler           /* IRQ57 DMA2_CH5                   */
  .word Default_Handler           /* IRQ58 DMA2_CH6                   */
  .word Default_Handler           /* IRQ59 DMA2_CH7                   */
  .word Default_Handler           /* IRQ60 DMAMUX1_OVR                */
  .size g_vectors, .-g_vectors

/* ── Reset handler ────────────────────────────────────────────────────── */
  .section .text.Reset_Handler
  .type Reset_Handler, %function
Reset_Handler:
  /* Copy .data section from FLASH to RAM */
  ldr   r0, =_sdata
  ldr   r1, =_edata
  ldr   r2, =_sidata
  movs  r3, #0
  b     LoopCopyData
CopyData:
  ldr   r4, [r2, r3]
  str   r4, [r0, r3]
  adds  r3, r3, #4
LoopCopyData:
  adds  r4, r0, r3
  cmp   r4, r1
  bcc   CopyData

  /* Zero-fill .bss section */
  ldr   r2, =_sbss
  ldr   r4, =_ebss
  movs  r3, #0
  b     LoopFillBss
FillBss:
  str   r3, [r2]
  adds  r2, r2, #4
LoopFillBss:
  cmp   r2, r4
  bcc   FillBss

  /* Call main() */
  bl    main

  /* Should never reach here */
Hang:
  b     Hang

  .size Reset_Handler, .-Reset_Handler

/* ── Default / infinite-loop handler ─────────────────────────────────── */
  .section .text.Default_Handler,"ax",%progbits
Default_Handler:
  b     Default_Handler
  .size Default_Handler, .-Default_Handler

/* ── Weak aliases — override by defining the real function in .c ──────── */
  .weak NMI_Handler
  .thumb_set NMI_Handler, Default_Handler

  .weak HardFault_Handler
  .thumb_set HardFault_Handler, Default_Handler

  .weak SVC_Handler
  .thumb_set SVC_Handler, Default_Handler

  .weak PendSV_Handler
  .thumb_set PendSV_Handler, Default_Handler

  .weak SysTick_Handler
  .thumb_set SysTick_Handler, Default_Handler

  .weak IPCC_C2_RX_IRQHandler
  .thumb_set IPCC_C2_RX_IRQHandler, Default_Handler
