# STM32F429IDISC1
![image](https://user-images.githubusercontent.com/2941745/161344856-648827dd-5bb5-4020-87b6-55a691b4f44c.png)

|   | STM32F429IDISC1 |
| ------------- | ------------- |
| Processor  | STM32F429ZI  |
| Processor family | ARM Cortex M4  |
| Flash  | 2MB internal flash |
| RAM  | Internal 256 KB + 4 KB SRAM  |
| External memory  | 64Mb SDRAM  |
| ADC  | 3 x 12-bit, 2.4 MSPS ADC  |
| Peripherals  | 2.4" QVGA TFT LCD, MEMs 3-axis gyro, 4x user LEDs, 1x user push button  |
| Dev Board Price  | ~$30 direct from ST  |
| Processor Price  | ~$20 direct from ST (currently out of stock like everything else)  |

# Application notes

An important requirement for most Flash memory-based systems is the ability to update the
firmware installed in the end product. This document provides general guidelines for
creating a firmware upgrade application based on the STM32F4DISCOVERY board.

The STM32F4 series microcontroller can run user-specific applications to upgrade the
firmware of the microcontroller-embedded Flash memory. This feature allows the use of any
type of communication protocol for the reprogramming process (for example, CAN, USART
and USB). USB Host mass storage is the example used in this application note.

The firmware upgrade using a USB Host is very advantageous because it is a standalone
executed code in which the user does not need to use a host computer to perform the
firmware upgrade. The user only needs a Flash disk to upgrade the target STM32 device.

https://www.st.com/resource/en/application_note/an3990-upgrading-stm32f4discovery-board-firmware-using-a-usb-key-stmicroelectronics.pdf
