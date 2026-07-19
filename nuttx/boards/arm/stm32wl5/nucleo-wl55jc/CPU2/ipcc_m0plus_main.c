#include <stdint.h>
#include <string.h>

/* ================= RCC ================= */
#define RCC_BASE        0x58000000u
#define RCC_AHB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x4C))
#define RCC_APB2ENR     (*(volatile uint32_t *)(RCC_BASE + 0x60))

#define RCC_GPIOBEN     (1u << 1)
#define RCC_USART1EN    (1u << 14)

/* ================= GPIOB ================= */
#define GPIOB_BASE      0x48000400u
#define GPIOB_MODER     (*(volatile uint32_t *)(GPIOB_BASE + 0x00))
#define GPIOB_AFRL      (*(volatile uint32_t *)(GPIOB_BASE + 0x20))

/* ================= USART1 ================= */
#define USART1_BASE     0x40013800u
#define USART1_CR1      (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART1_BRR      (*(volatile uint32_t *)(USART1_BASE + 0x0C))
#define USART1_ISR      (*(volatile uint32_t *)(USART1_BASE + 0x1C))
#define USART1_TDR      (*(volatile uint32_t *)(USART1_BASE + 0x28))

#define UE              (1u << 0)
#define TE              (1u << 3)
#define RE              (1u << 2)
#define TXE             (1u << 7)
#define TC              (1u << 6)

/*==============USART2========================*/
#define USART2_BASE     0x40004400u
#define USART2_CR1      (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART2_BRR      (*(volatile uint32_t *)(USART2_BASE + 0x0C))
#define USART2_ISR      (*(volatile uint32_t *)(USART2_BASE + 0x1C))
#define USART2_TDR      (*(volatile uint32_t *)(USART2_BASE + 0x28))

/* ================= IPCC (CONFIRMED OFFSETS) ================= */
#define IPCC_BASE        0x58000C00u

#define IPCC_C1CR        (*(volatile uint32_t *)(IPCC_BASE + 0x00))
#define IPCC_C1MR        (*(volatile uint32_t *)(IPCC_BASE + 0x04))
#define IPCC_C1SCR       (*(volatile uint32_t *)(IPCC_BASE + 0x08))
#define IPCC_C1TOC2SR    (*(volatile uint32_t *)(IPCC_BASE + 0x0C))

#define IPCC_C2CR        (*(volatile uint32_t *)(IPCC_BASE + 0x10))
#define IPCC_C2MR        (*(volatile uint32_t *)(IPCC_BASE + 0x14))
#define IPCC_C2SCR       (*(volatile uint32_t *)(IPCC_BASE + 0x18))
#define IPCC_C2TOC1SR    (*(volatile uint32_t *)(IPCC_BASE + 0x1C))

#define IPCC_CHAN1_BIT   (1u << 0)        /* channel 1, bit 0 */

/* ================= Shared memory (NuttX layout) ================= */
#define IPCC_CHAN1_RX_SIZE  256u
#define IPCC_START          0x20008000u

#define IPCC_RX_LEN   (*(volatile uint32_t *)(IPCC_START))
#define IPCC_RX_DATA  ((volatile uint8_t *)(IPCC_START + 4))

#define IPCC_TX_LEN   (*(volatile uint32_t *)(IPCC_START + IPCC_CHAN1_RX_SIZE))
#define IPCC_TX_DATA  ((volatile uint8_t *)(IPCC_START + IPCC_CHAN1_RX_SIZE + 4))

/* ================= UART ================= */
static void uart_putc(char c)
{
  while (!(USART1_ISR & TXE));
  USART1_TDR = (uint8_t)c;
}

static void uart_puts(const char *s)
{
  while (*s)
  {
    if (*s == '\n')
      uart_putc('\r');
    uart_putc(*s++);
  }
  while (!(USART1_ISR & TC));
}

static void uart_init(void)
{
  RCC_AHB2ENR |= RCC_GPIOBEN;
  RCC_APB2ENR |= RCC_USART1EN;

  GPIOB_MODER &= ~((3u << (6 * 2)) | (3u << (7 * 2)));
  GPIOB_MODER |=  ((2u << (6 * 2)) | (2u << (7 * 2)));

  GPIOB_AFRL &= ~((0xFu << 24) | (0xFu << 28));
  GPIOB_AFRL |=  ((7u   << 24) | (7u   << 28));

  USART1_BRR = 417;   /* 48MHz / 115200 */
  USART1_CR1 = UE | TE | RE;
}

/* ================= IPCC =================
 * NOTE: NOT enabling RCC IPCC clock here -- NuttX (M4) already
 * enables it system-wide before M0+ boots. We only touch the
 * C2 (our own) control/mask/status registers.
 */
static void ipcc_init(void)
{
  /* Mask all C2 interrupts -- we are polling, not using IRQ */
  IPCC_C2MR = 0xFFFFFFFFu;
}

static int ipcc_rx_ready(void)
{
  return (IPCC_C1TOC2SR & IPCC_CHAN1_BIT) != 0;
}

static void ipcc_rx_read(char *buf, uint32_t bufsize)
{
  uint32_t len = IPCC_RX_LEN;
  if (len >= bufsize)
    len = bufsize - 1;

  for (uint32_t i = 0; i < len; i++)
    buf[i] = (char)IPCC_RX_DATA[i];

  buf[len] = '\0';

  /* Clear channel 1 bit -> tells M4 we consumed it (SCR bit 0 = clear) */
  IPCC_C2SCR = IPCC_CHAN1_BIT;
}

static void ipcc_tx_send(const char *msg)
{
  uint32_t len = (uint32_t)strlen(msg);

  IPCC_TX_LEN = len;
  for (uint32_t i = 0; i < len; i++)
    IPCC_TX_DATA[i] = (uint8_t)msg[i];

  /* Set channel 1 bit in TX direction (SCR bit 16 = set) */
  IPCC_C2SCR = (IPCC_CHAN1_BIT << 16);
}

/* ================= MAIN ================= */
int main(void)
{
  char rxbuf[256];

  uart_init();
  uart_puts("\r\nM0+ BOOT OK\r\n");

  ipcc_init();
  uart_puts("M0+ IPCC INIT DONE\r\n");

  while (1)
  {
    if (ipcc_rx_ready())
    {
      uart_puts("M0+ SAW RX FLAG\r\n");

      ipcc_rx_read(rxbuf, sizeof(rxbuf));

      uart_puts("M0+ RX from M4: ");
      uart_puts(rxbuf);
      uart_puts("\r\n");

      ipcc_tx_send("Hello from M0+");

      uart_puts("M0+ TX reply sent\r\n");
    }
  }
}