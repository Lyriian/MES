# Week 3 assignment - Make Blinky

On your final project board1, make blinky for yourself. Then add a button to turn the LED on and
off. Bonus points for making the button cause an interrupt. Triple bonus points for debouncing
the button signal.
What build environment are you using? You have many free options: STM32CubeMxIDE,
VSCode with Platformio, Platformio, Keil (free version), Arm GNU tools, and so on. For many of
these, there are examples, explore these examples and re-use HALs and code as you can.
Can you step through the code to see what each line does?
Investigate further, using the processor manual:
● What are the hardware registers that cause the LED to turn on and off? (From the
processor manual, don’t worry about initialization.)
● What are the registers that you read in order to find out the state of the button?
● Can you read the register directly and see the button change in a debugger or by
printing out the value of the memory at the register’s address?
Turn in your code with a comment or additional file answering the questions
