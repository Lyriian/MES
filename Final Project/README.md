
# Card cycle tester

## Overview
I had a different project in mind when we started this class but it would require a bit more time that what we have left of class. So instead I thought it would be a good idea to improve an old project I had done for my job. We make Point of Sale systems and with that it means lots of card reader devices. One thing we like to verify on all the card readers is that the swipe and dip slots for the cards can live up to repeated use. So early on when I started there I hacked together a very janky card cycle tester using some actuators and an arduino. It's been used quite a bit over the years and despite how terrible of a setup it is it gets the job done. So for the final project I'd like to condense the systems down to a single board with no external controls or monitors. 

### Old setup
Below is our old setup. The main components here are the Arduino, L298N motor controller, a 12V actuator and a 12V switching power supply (out of frame). The code on the arduino is very simple and just toggles two GPIOs to control the push / pull direction on the actuator. There's a delay set to control how long to hold either the push or pull direction.

The messy part of this system right now is that we needed a way to read the counter for how many cycles have been completed. Since originally I didn't have a ton of time and I happened to have a raspberry pi laying around... that ended up being the solution. I used putty to open a serial connection between the Pi and the arduino. This let me just dump the counter out to the serial port and also had the added benefit of giving some direct control via console commands. The Pi also served as a nice hub to conntect to multiple arduinos all running different Cycle testers in slightly different configurations and I was able to modify code if needed from the Pi. 

![annotated cycle tester](https://user-images.githubusercontent.com/2941745/167262852-5c5f3b19-d037-494a-a7e6-58a828c51e95.png)

### The goal of this project
- Everything in the next version needed to require minimal input from a technician. 
- Parameters of tests should be easily configureable without having to re-write any code. 
- Test status also needs to be visible without opening a serial terminal and connecting from another device.
- Eventually create a single PCB to attach in an enclosure on the 80/20 extrusion frame of the tester.
- Learn to make the step from arduino to something more complex. 

## Hardware description
The following hardware will be used:
- STM32F429 Discovery board
- 16x02 LCD display with I2C bridge
- L298n H-bridge motor driver
- Rotary encoder with a push button switch
- 12V linear actuator
- 12V power supply

### Hardware Block Diagram
![image](https://user-images.githubusercontent.com/2941745/173286399-57c421c3-7612-4e02-a54c-3749826dfe0a.png)

## Software Description

The software is fairly simple. When powered on the LCD will display menu items beginning with "Start test". Using the encoder knob you can scroll through the menu which well present you with the different parameters of a test that you can edit. These parameters for the moment are "Push time" which sets the duration the motor will extend for. "Pull time" which sets the amount of time the motor will retract. "Cycle limit" which will set the maximum number for cycles for your test. 

### Software Parts


|Name|Purpose|
|----|-------|
| main.c | Includes main program loop and all functions related to system control|
| I2C_LCD16x02.c| includes all functions related to the LCD initialization and control|
| Encoder.c | sets up the GPIO and timers for the rotary encoder knob|
| ConsoleCommands.c | for debugging via the serial connection |

### State Machine
![image](https://user-images.githubusercontent.com/2941745/173298914-3949f89a-0050-4939-bc37-68b8963ab57b.png)

### Software block diagram
![image](https://user-images.githubusercontent.com/2941745/173299170-bc3b7313-16c8-454e-9457-947393a500b8.png)

## Build instructions

### Hardware
The L298N motor board has two H-bridge bridge circuits which can be used to control the direction of two separate motors. I'm only using on for this project so we only need two of the input lines. I'm using inputs 1 and 2 and they are connected to GPIOs PD_5 and PD_6 on the discovery board. I'm powering the driver board from a 12V wall adaptor that I've used a barrle jack breakout to wire it up to the VS and GND pins. GND from both the discovery board and the LCD also tie in here. The motor wires up to OUT1 and OUT2 of this board. This motor board also has an onboard 5V regulator which steps down the 12V from the wall adaptor and is used as a reference for the input loging. It it also the 5V source I'm using to power the LCD, Discovery board and encoder.

The LCDs SCL line is connected to PB_6 and SDA PB_7.

For the encoder if using three GPIO. The switch is wired to PD_7 and the remaining two GPIO are the clock pins. I'm using timer2 in encoder mode, the CLK pin on my encoder is connected to PA_5 which is CHA1 and the DT pin is connected to PB_3 which is CHA2 of the timer. 

![image](https://user-images.githubusercontent.com/2941745/173307050-4440c4e0-e901-412a-9053-912de49bd16b.png)

![image](https://user-images.githubusercontent.com/2941745/173303517-6a4ba60f-a762-45f3-b41f-7c20d2d5ab3a.png)

![image](https://user-images.githubusercontent.com/2941745/173303566-2376178e-9ae0-4e94-806e-e1ca0e5f0705.png)

![image](https://user-images.githubusercontent.com/2941745/173303764-31b937d0-c52a-4ef4-a167-8d0cbfe9a435.png)

### Software
For this project I used VSCode with the PlatformIO extension to setup the project using the STM32Cube framework. 

### reused code
I used the console homework as my starting point for this project since I knew I'd have to implement it anyways and the commands were useful for debugging as I got each piece online. The LCD library was written by Sharath N. and can be found here (https://github.com/SharathN25/LDC16x02_I2C_Driver_STM32F407) in it's entirety. I only made slight modifications to it but I cut out most of the spacing and comments for better viewing for my benefit. 

## Future Improvements
Right now this is an incredibly simple project and it's exactly impressive. It's a machine that moves a motor to swipe a card in a card reader with the intent of destroying said reader after 1000s of cycles. I only intended to mimic my previous setups functions with a less complicated interface for technicians. That being said as I was getting this together I thought of several things I'd like to add before I turn this into an actual board. 

First I need to port the code over to a pi pico. I'd like to use the RP2040 in my final design as it's cheap, in stock and this isn't a complex program so the power and features of the F4 just go to waste. 

I'd like to also add persistent data to this either via and SD card or just using onboard flash. Currently I set a default program by initializing variables at power on. I'd rather be able to have whatever the last edited settings were be written to memory so they can be recalled after power cycling and also add the ability to store additional test parameters so that various timings can be saved and recalled quickly without having to reconfigure them from 0 every time. 

A different motor driver might also be in my future the H-bridge has approximately a 2V drop across it so they 12V going in as a source is actually only 10V at the motor. This limits the actuators maximum speed which is already not spectacular and while this motor board could support PWM for speed control this setup was so slow that there was really no reason to use PWM as it would only furthur slow the already turtle paced motor. 

There's also one last feature I'd love to add which is to get feedback from the payment device. At the moment this machine exists just to destroy card readers but if I could get the card reader to send any sort of feedback signal I could engage disengage the actuator based on that and fully automate a payments workflow. I several ideas of how to implement that but due to PCI compliance issues on card readers it's something I'd need to work my some of the other engineering teams at my office to accomplish. 

## Conclusion
I realized quickly during this course that my initial project idea was way too grandiose considering I have not done much programming since college over a decade ago. I pivoted to this project because it was beneficial to me and my job as well as being something I was familliar with the requirements for so I could focus on learning the tool chains, system architecture, and somedesign process stuff rather than getting way over my head in low level code that I have no real understanding of. My main goal for this course was to figure out how to bridge the gap between exclusively using the arduino ecosystem to working with chips that are more likely to end up in a shipping product as well as just learn what firmware engineers do with boards I hand off to them. 

I definitely feel like I have a significantly better understanding of embedded design as a whole and I love the fact that the code to define a single GPIO pin is longer than the entire arduino sketch I wrote for the original tester here. I also feel like I've finally figured out version control. I never really used GIT before this course other than cloning some Repos but now I don't know how I've lived without this. I've got a very long list of new topics to look into as well as a lot of recommended reference material from everyone. This class has been great and I thank everyone involved with it. 

## Grading (self-assessment)
| Criteria | Self Grade | Comments |
|----------|------------|----------|
|Meets Minimum Goals| 2 | I2C for the LCD, UART for the console, GPIO timer for encoder, and two push buttons between the encoder switch and the on board button as an E-STOP|
|Completeness of deliverables| 2 | It's all there but I honestly wish I'd had a little more time to add some things like writing to an SD card|
|Clear intentions and working code| 2 | this functions but I look at my code and I'm displeased. I kind of learned how structures should look as I progressed so sometimes I'm naming inputs other times they're just the gpio number. Definitely need to brush up on coding best practices|
|Reusing code| 2 | all of the CLI code was from the previous porting assignment other than some new commands, the LCD library is one I found. Main and Encoder are my own code|
|Originality and scope of goals| 1 | honestly it's as bare bones as it gets, I was blown away by other students projects and while I had better initial ideas I just realized early on that I didn't have the time of knowledge of execute them during this course. Though I do think having done this I know where to start now for this bigger projects.

## Video Demo
https://youtu.be/sdnc3Wkv_rY
