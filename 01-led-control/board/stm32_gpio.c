/****************************************************************************
 * boards/arm/stm32/stm32f4discovery/src/stm32_gpio.c
 *
 * STM32F4Discovery GPIO Driver
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <syslog.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/ioexpander/gpio.h>

#include "chip.h"
#include "stm32_gpio.h"
#include "stm32f4discovery.h"

#if defined(CONFIG_DEV_GPIO) && !defined(CONFIG_GPIO_LOWER_HALF)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* LED GPIO pinleri */
#ifndef GPIO_LED1
#  define GPIO_LED1 (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                     GPIO_OUTPUT_CLEAR | GPIO_PORTD | GPIO_PIN12)
#endif

#ifndef GPIO_LED2
#  define GPIO_LED2 (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                     GPIO_OUTPUT_CLEAR | GPIO_PORTD | GPIO_PIN13)
#endif

#ifndef GPIO_LED3
#  define GPIO_LED3 (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                     GPIO_OUTPUT_CLEAR | GPIO_PORTD | GPIO_PIN14)
#endif

#ifndef GPIO_LED4
#  define GPIO_LED4 (GPIO_OUTPUT | GPIO_PUSHPULL | GPIO_SPEED_50MHz | \
                     GPIO_OUTPUT_CLEAR | GPIO_PORTD | GPIO_PIN15)
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

struct stm32gpio_dev_s
{
  struct gpio_dev_s gpio;
  uint8_t id;
};

struct stm32gpioout_dev_s
{
  struct stm32gpio_dev_s stm32gpio;
  uint32_t pin;
};

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

static int gpout_read(FAR struct gpio_dev_s *dev, FAR bool *value);
static int gpout_write(FAR struct gpio_dev_s *dev, bool value);

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const struct gpio_operations_s gpout_ops =
{
  .go_read   = gpout_read,
  .go_write  = gpout_write,
  .go_attach = NULL,
  .go_enable = NULL,
};

/* LED pinleri */
static const uint32_t g_gpiooutputs[4] =
{
  GPIO_LED1,  /* /dev/gpio0 - PD12 Yeşil */
  GPIO_LED2,  /* /dev/gpio1 - PD13 Turuncu */
  GPIO_LED3,  /* /dev/gpio2 - PD14 Kırmızı */
  GPIO_LED4,  /* /dev/gpio3 - PD15 Mavi */
};

static struct stm32gpioout_dev_s g_gpout[4];

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static int gpout_read(FAR struct gpio_dev_s *dev, FAR bool *value)
{
  FAR struct stm32gpioout_dev_s *stm32gpout = 
    (FAR struct stm32gpioout_dev_s *)dev;

  DEBUGASSERT(stm32gpout != NULL && value != NULL);
  DEBUGASSERT(stm32gpout->stm32gpio.id < 4);
  
  *value = stm32_gpioread(stm32gpout->pin);
  return OK;
}

static int gpout_write(FAR struct gpio_dev_s *dev, bool value)
{
  FAR struct stm32gpioout_dev_s *stm32gpout = 
    (FAR struct stm32gpioout_dev_s *)dev;

  DEBUGASSERT(stm32gpout != NULL);
  DEBUGASSERT(stm32gpout->stm32gpio.id < 4);
  
  stm32_gpiowrite(stm32gpout->pin, value);
  return OK;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int stm32_gpio_initialize(void)
{
  int i;
  int ret;
  char devname[16];

  for (i = 0; i < 4; i++)
    {
      /* Pin'i yapılandır */
      stm32_configgpio(g_gpiooutputs[i]);

      /* Cihaz yapısını başlat */
      g_gpout[i].stm32gpio.gpio.gp_pintype = GPIO_OUTPUT_PIN;
      g_gpout[i].stm32gpio.gpio.gp_ops     = &gpout_ops;
      g_gpout[i].stm32gpio.id              = i;
      g_gpout[i].pin                       = g_gpiooutputs[i];

      /* Cihazı kaydet */
      snprintf(devname, sizeof(devname), "/dev/gpio%d", i);
      ret = gpio_pin_register(&g_gpout[i].stm32gpio.gpio, i);
      
      if (ret < 0)
        {
          syslog(LOG_ERR, "ERROR: gpio_pin_register() failed: %d\n", ret);
          return ret;
        }

      syslog(LOG_INFO, "Registered %s\n", devname);
    }

  return OK;
}

#endif /* CONFIG_DEV_GPIO && !CONFIG_GPIO_LOWER_HALF */
