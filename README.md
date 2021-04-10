# pico-stepper-counter
Simple project for Raspberry Pi PICO, which counts steps send to stepper motors.

If for example connected to an Arduino running GRBL, it counts the pulses send to the stepper motors. The position calculated from these pulses is output over the USB connection.

The number of steps per mm must be configured in the const MM_PER_STEP. When using 40 steps per mm, each step is 0.025 mm (1000 / 40).

The connections to the microcontroller controlling the stepper motors also checks the direction to add or subtract the steps from the position.

Check if you need to adjust the voltage level from the output signals, because the PICO is not 5 Volt tolerant. I've used some  simple resitors (330Ω and 670Ω) I had laying around to make a voltage divider.
