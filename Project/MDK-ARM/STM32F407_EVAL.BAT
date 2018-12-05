SET PATH=c:\Program Files\ARM\BIN40;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Program Files\Intel\WiFi\bin\;C:\Program Files\Common Files\Intel\WirelessCommon\;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin\;C:\Program Files\IVI Foundation\VISA\Win64\Bin\;C:\Program Files (x86)\IVI Foundation\VISA\WinNT\Bin;C:\Program Files\Intel\WiFi\bin\;C:\Program Files\Common Files\Intel\WirelessCommon\
SET ARMCC41INC=c:\Program Files\ARM\RV31\Inc
SET ARMCC41LIB=c:\Program Files\ARM\RV31\Lib
SET CPU_TYPE=STM32F407VG
SET CPU_VENDOR=STMicroelectronics
SET UV2_TARGET=STM32F407_EVAL
SET CPU_CLOCK=0x017D7840
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\main.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_it.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\misc.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usb_bsp.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_desc.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_storage_template.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_usr.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\udisk.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\flash_if.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\bsp_nand_flash.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\system_stm32f4xx.__i"
"c:\Program Files\ARM\BIN40\ArmAsm" --Via ".\stm32f407-eval\startup_stm32f40xx._ia"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_exti.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_gpio.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_pwr.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_rcc.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_syscfg.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_spi.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_flash.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\stm32f4xx_fsmc.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\delay.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usb_core.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usb_dcd.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usb_dcd_int.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_core.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_ioreq.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_req.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_msc_bot.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_msc_core.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_msc_data.__i"
"c:\Program Files\ARM\BIN40\ArmCC" --Via ".\stm32f407-eval\usbd_msc_scsi.__i"
"c:\Program Files\ARM\BIN40\ArmLink" --Via ".\STM32F407-EVAL\STM32F407-EVAL.lnp"
c:\Program Files\ARM\BIN40\fromelf.exe ".\STM32F407-EVAL\STM32F407-EVAL.axf" --i32combined --output ".\STM32F407-EVAL\STM32F407-EVAL.hex"
fromelf --bin --output .\obj\11.bin .\STM32F407-EVAL\STM32F407-EVAL.axf
