#include <stdint.h>


/* =====================================================
   RCC
   ===================================================== */

#define RCC_BASE        0x58000000UL

#define RCC_AHB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x4C))
#define RCC_APB1ENR1    (*(volatile uint32_t *)(RCC_BASE + 0x58))
#define RCC_CCIPR       (*(volatile uint32_t *)(RCC_BASE + 0x88))
#define RCC_APB1RSTR1   (*(volatile uint32_t *)(RCC_BASE + 0x38))


#define RCC_GPIOAEN     (1 << 0)
#define RCC_USART2EN    (1 << 17)



/* =====================================================
   GPIOA
   ===================================================== */

#define GPIOA_BASE      0x48000000UL

#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_OTYPER    (*(volatile uint32_t *)(GPIOA_BASE + 0x04))
#define GPIOA_OSPEEDR   (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_PUPDR     (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_AFRL      (*(volatile uint32_t *)(GPIOA_BASE + 0x20))



/* =====================================================
   USART2
   ===================================================== */

#define USART2_BASE     0x40004400UL

#define USART2_CR1      (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART2_BRR      (*(volatile uint32_t *)(USART2_BASE + 0x0C))
#define USART2_ISR      (*(volatile uint32_t *)(USART2_BASE + 0x1C))
#define USART2_TDR      (*(volatile uint32_t *)(USART2_BASE + 0x28))


#define USART_CR1_UE    (1 << 0)
#define USART_CR1_TE    (1 << 3)

#define USART_ISR_TXE   (1 << 7)
#define USART_ISR_TC    (1 << 6)



/* =====================================================
   UART INIT
   ===================================================== */

void uart2_init(void)
{

    /*
       Enable GPIOA clock
    */

    RCC_AHB2ENR |= RCC_GPIOAEN;


    /*
       Enable USART2 clock
    */

    RCC_APB1ENR1 |= RCC_USART2EN;


    /*
       Select USART2 clock source = PCLK1
       00 = PCLK
    */

    RCC_CCIPR &= ~(3 << 2);



    /*
       Reset USART2
    */

    RCC_APB1RSTR1 |= RCC_USART2EN;
    RCC_APB1RSTR1 &= ~RCC_USART2EN;



    /*
       PA2 PA3 Alternate function mode
    */

    GPIOA_MODER &= ~((3<<(2*2)) |
                     (3<<(3*2)));

    GPIOA_MODER |=  ((2<<(2*2)) |
                     (2<<(3*2)));



    /*
       AF7 USART2
    */

    GPIOA_AFRL &= ~((0xF<<8) |
                    (0xF<<12));

    GPIOA_AFRL |=  ((7<<8) |
                    (7<<12));



    /*
       Output speed high
    */

    GPIOA_OSPEEDR |= (3<<(2*2));
    GPIOA_OSPEEDR |= (3<<(3*2));


    /*
       Push pull
    */

    GPIOA_OTYPER &= ~(1<<2);
    GPIOA_OTYPER &= ~(1<<3);



    /*
       Pull up
    */

    GPIOA_PUPDR &= ~((3<<(2*2)) |
                     (3<<(3*2)));

    GPIOA_PUPDR |= ((1<<(2*2)) |
                    (1<<(3*2)));



    /*
       Baud rate

       Clock = 48MHz
       Baud = 115200

       BRR = 48000000 / 115200
           = 416.66

    */

    USART2_BRR = 417;



    /*
       Enable transmitter
       Enable USART
    */

    USART2_CR1 = USART_CR1_TE;

    USART2_CR1 |= USART_CR1_UE;


}



/* =====================================================
   Send one character
   ===================================================== */

void uart2_putc(char c)
{

    while(!(USART2_ISR & USART_ISR_TXE));


    USART2_TDR = c;

}



/* =====================================================
   Send string
   ===================================================== */

void uart2_puts(const char *s)
{

    while(*s)
    {

        if(*s == '\n')
        {
            uart2_putc('\r');
        }


        uart2_putc(*s++);

    }


    while(!(USART2_ISR & USART_ISR_TC));

}



/* =====================================================
   Delay
   ===================================================== */

void delay(void)
{

    for(volatile uint32_t i=0;i<800000;i++)
    {

    }

}



/* =====================================================
   Main CPU2
   ===================================================== */

int main(void)
{


    uart2_init();


    while(1)
    {

        uart2_puts(
        "Hello from STM32WL55 Cortex-M0+ USART2\r\n"
        );


        delay();

    }


    return 0;

}