/****************************************************************************
 * examples/led_control/led_control_main.c
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <nuttx/ioexpander/gpio.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/* STM32F4Discovery LED pinleri */
#define LED_GREEN   0  /* PD12 - /dev/gpio0 */
#define LED_ORANGE  1  /* PD13 - /dev/gpio1 */
#define LED_RED     2  /* PD14 - /dev/gpio2 */
#define LED_BLUE    3  /* PD15 - /dev/gpio3 */

#define NUM_LEDS    4
#define DELAY_MS    500

/****************************************************************************
 * Private Data
 ****************************************************************************/

static const char *led_names[] =
{
  "Yeşil (PD12)",
  "Turuncu (PD13)",
  "Kırmızı (PD14)",
  "Mavi (PD15)"
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: led_set
 *
 * Description:
 *   GPIO kullanarak LED'i aç veya kapat
 *
 ****************************************************************************/

static void led_set(int fd, bool on)
{
  ioctl(fd, GPIOC_WRITE, (unsigned long)(on ? 1 : 0));
}

/****************************************************************************
 * Name: show_led_pattern
 *
 * Description:
 *   LED'leri sırayla yak/söndür
 *
 ****************************************************************************/

static void show_led_pattern(int leds[])
{
  int i;
  
  printf("LED kontrolü başlatılıyor...\n");
  printf("Çıkmak için Ctrl+C tuşlayın\n\n");
  
  /* Tüm LED'leri söndür */
  for (i = 0; i < NUM_LEDS; i++)
    {
      led_set(leds[i], false);
    }
  
  /* Ana döngü */
  while (1)
    {
      for (i = 0; i < NUM_LEDS; i++)
        {
          printf("%s LED yanıyor...\n", led_names[i]);
          led_set(leds[i], true);
          usleep(DELAY_MS * 1000);
          led_set(leds[i], false);
          usleep(100 * 1000);
        }
      printf("\n");
    }
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * led_control_main
 ****************************************************************************/

int main(int argc, FAR char *argv[])
{
  int leds[NUM_LEDS];
  int i;
  bool all_opened = true;
  
  printf("\n");
  printf("╔════════════════════════════════════════╗\n");
  printf("║  STM32F4Discovery LED Kontrol (GPIO)  ║\n");
  printf("║         NuttX RTOS Kullanımı          ║\n");
  printf("╚════════════════════════════════════════╝\n");
  printf("\n");
  
  /* GPIO cihazlarını aç */
  printf("GPIO cihazları açılıyor...\n");
  
  leds[LED_GREEN]  = open("/dev/gpio0", O_RDWR);
  leds[LED_ORANGE] = open("/dev/gpio1", O_RDWR);
  leds[LED_RED]    = open("/dev/gpio2", O_RDWR);
  leds[LED_BLUE]   = open("/dev/gpio3", O_RDWR);
  
  /* Tüm GPIO'ların açıldığını kontrol et */
  for (i = 0; i < NUM_LEDS; i++)
    {
      if (leds[i] < 0)
        {
          fprintf(stderr, "HATA: /dev/gpio%d açılamadı (errno: %d)\n", 
                  i, leds[i]);
          all_opened = false;
        }
      else
        {
          printf("✓ /dev/gpio%d başarıyla açıldı (%s)\n", i, led_names[i]);
        }
    }
  
  if (!all_opened)
    {
      fprintf(stderr, "\n");
      fprintf(stderr, "GPIO cihazları bulunamadı!\n");
      fprintf(stderr, "Çözüm:\n");
      fprintf(stderr, "1. menuconfig'de GPIO driver etkin mi kontrol edin\n");
      fprintf(stderr, "2. Board dosyasında GPIO pinleri tanımlı mı kontrol edin\n");
      fprintf(stderr, "   (boards/arm/stm32/stm32f4discovery/src/stm32_gpio.c)\n");
      
      /* Açılanları kapat */
      for (i = 0; i < NUM_LEDS; i++)
        {
          if (leds[i] >= 0)
            {
              close(leds[i]);
            }
        }
      
      return EXIT_FAILURE;
    }
  
  printf("\nTüm GPIO cihazları başarıyla açıldı!\n\n");
  
  /* LED pattern'ini göster */
  show_led_pattern(leds);
  
  /* Temizlik (buraya hiç gelmeyecek ama yine de) */
  for (i = 0; i < NUM_LEDS; i++)
    {
      led_set(leds[i], false);
      close(leds[i]);
    }
  
  return EXIT_SUCCESS;
}
