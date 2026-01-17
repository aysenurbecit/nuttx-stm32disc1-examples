# GPIO üzerinden LED Kontrolü

STM32F4Discovery kartındaki 4 LED'i GPIO kullanarak sırayla yakıp söndüren NuttX uygulaması.

## Gereksinimler

- NuttX workspace
- ARM GCC toolchain
- st-link tools

## Kurulum

### 1) Dosyaları Kopyalayın

Dosyaları Nuttx altındaki ilgili dizinlere kopyalayın.

```bash

# Uygulama dosyaları
cp -r /path/to/nuttx-stm32disc1-examples/01-led-control/app/* \
    apps/examples/led_control/

# Board dosyasını kopyala
cp /path/to/nuttx-stm32disc1-examples/01-led-control/board/stm32_gpio.c \
    nuttx/boards/arm/stm32/stm32f4discovery/src/
```

### 2) apps/examples Değişiklikleri

**apps/examples/Kconfig** dosyasının sonuna ekleyin:
```
source "examples/led_control/Kconfig"
```

**apps/examples/Make.defs** dosyasına ekleyin:
```makefile
ifeq ($(CONFIG_EXAMPLES_LED_CONTROL),y)
CONFIGURED_APPS += $(APPDIR)/examples/led_control
endif
```

### 3) Board Değişiklikleri

**boards/arm/stm32/stm32f4discovery/src/Makefile** dosyasına ekleyin:
```makefile
ifeq ($(CONFIG_DEV_GPIO),y)
  CSRCS += stm32_gpio.c
endif
```

**boards/arm/stm32/stm32f4discovery/src/stm32_bringup.c** dosyasına:

Fonksiyon prototipi ekleyin:
```c
#ifdef CONFIG_DEV_GPIO
int stm32_gpio_initialize(void);
#endif
```

`stm32_bringup()` fonksiyonunun sonuna ekleyin:
```c
#ifdef CONFIG_DEV_GPIO
  ret = stm32_gpio_initialize();
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: Failed to initialize GPIO: %d\n", ret);
    }
#endif
```

### 4) Konfigürasyon

```bash
cd ~/nuttx_ws/nuttx

make distclean
./tools/configure.sh stm32f4discovery:nsh

# Menuconfig
make menuconfig
```

**Menuconfig'de şu ayarları yapılmalı:**

1. **Device Drivers** → **IO Expander/GPIO Support** → **[*] GPIO driver**
2. **Application Configuration** → **Examples** → **[*] LED Control example**
3. **System Type** → **STM32 Peripheral Support** → **[*] USART2**


### 5) Derleme

```bash
make clean
make -j$(nproc)
```

### Adım 6: Flash

```bash
sudo st-flash write nuttx.bin 0x8000000
```

## Donanım Bağlantıları

### USART2 (Debug - Opsiyonel)
```
STM32 PA2 (TX) → USB-Serial RX
STM32 PA3 (RX) → USB-Serial TX
STM32 GND → USB-Serial GND
```

## Kullanım

### 1. Seri Port Bağlantısı

```bash
sudo minicom -D /dev/ttyUSB0 -b 115200
```
Reset tuşuna basına ardından gelen terminalde uygulamayı çalıştırın.

```bash
nsh> led_control
```

### Output

```
╔════════════════════════════════════════╗
║  STM32F4Discovery LED Kontrol (GPIO)  ║
╚════════════════════════════════════════╝

GPIO cihazları başarıyla açıldı.
LED kontrolü başlatılıyor...
Çıkmak için Ctrl+C tuşlayın

Yeşil (PD12) LED yanıyor...
Turuncu (PD13) LED yanıyor...
Kırmızı (PD14) LED yanıyor...
Mavi (PD15) LED yanıyor...
```

### Derleme hatası

```bash
# Temiz derleme
cd ~/path/to/nuttx
make distclean
./tools/configure.sh stm32f4discovery:nsh
# Konfigürasyonu tekrarla
make -j$(nproc)
```
