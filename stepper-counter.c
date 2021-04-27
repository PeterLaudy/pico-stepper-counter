#include <stdio.h>
#include <pico/stdlib.h>
#include <pico/multicore.h>
#include <hardware/irq.h>
#include <hardware/gpio.h>
#include <hardware/structs/iobank0.h>

/*
 * The resolution of the stepper motors and spindle.
 * 500 steps per mm gives us a step of 1000/500 = 0.002 mm each.
 */
const int MM_PER_STEP = (1000 / 500);

/*
 * The pin-layout.
 */
const uint X_DIR_PIN = 0;
const uint X_STEP_PIN = 1;

const uint Y_DIR_PIN = 2;
const uint Y_STEP_PIN = 3;

const uint Z_DIR_PIN = 4;
const uint Z_STEP_PIN = 5;

const uint A_DIR_PIN = 6;
const uint A_STEP_PIN = 7;

const uint B_DIR_PIN = 8;
const uint B_STEP_PIN = 9;

const uint C_DIR_PIN = 10;
const uint C_STEP_PIN = 11;

const uint IRQ_TIMING_PIN = 13;

/*
 * The counters for the three stepper motors.
 */
volatile int x_step_count = 0;
volatile int y_step_count = 0;
volatile int z_step_count = 0;
volatile int a_step_count = 0;
volatile int b_step_count = 0;
volatile int c_step_count = 0;

/*
 * The interrupt routine which is triggered by each pulse for any of the three stepper motors.
 */
void __no_inline_not_in_flash_func(step_irq_handler)() {

    gpio_set_mask(1u << IRQ_TIMING_PIN);

    uint32_t gpio0 = iobank0_hw->proc1_irq_ctrl.ints[0];
    uint32_t gpio1 = iobank0_hw->proc1_irq_ctrl.ints[1];

    iobank0_hw->intr[0] = gpio0;
    iobank0_hw->intr[1] = gpio1;

    uint32_t edgeHighMask = 0x80u;

    // Check if the stepper motor for the X axis was pulsed.
    if (gpio0 & edgeHighMask) {
        // Depending on the direction we increase or decrease the position.
        x_step_count += gpio_get(X_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    edgeHighMask <<= 8;

    // Check if the stepper motor for the Y axis was pulsed.
    if (gpio0 & edgeHighMask) {
        // Depending on the direction we increase or decrease the position.
        y_step_count += gpio_get(Y_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    edgeHighMask <<= 8;

    // Check if the stepper motor for the Z axis was pulsed.
    if (gpio0 & edgeHighMask) {
        // Depending on the direction we increase or decrease the position.
        z_step_count += gpio_get(Z_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    edgeHighMask <<= 8;

    // Check if the stepper motor for the A axis was pulsed.
    if (gpio0 & edgeHighMask) {
        // Depending on the direction we increase or decrease the position.
        a_step_count += gpio_get(A_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    edgeHighMask = 0x80u;

    // Check if the stepper motor for the B axis was pulsed.
    if (gpio1 & edgeHighMask) {
        // Depending on the direction we increase or decrease the position.
        b_step_count += gpio_get(B_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    edgeHighMask <<= 8;

    // Check if the stepper motor for the C axis was pulsed.
    if (gpio1 & edgeHighMask) {
        // Depending on the direction we increase or decrease the position.
        c_step_count += gpio_get(C_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    gpio_clr_mask(1u << IRQ_TIMING_PIN);

}

void core1Code() {
    // Initialize the interrupt timing pin.
    gpio_init(IRQ_TIMING_PIN);
    gpio_set_dir(IRQ_TIMING_PIN, GPIO_OUT);
    gpio_put(IRQ_TIMING_PIN, false);

    // Set the direction pins as INPUT.
    gpio_init(X_DIR_PIN);
    gpio_set_dir(X_DIR_PIN, GPIO_IN);

    gpio_init(Y_DIR_PIN);
    gpio_set_dir(Y_DIR_PIN, GPIO_IN);

    gpio_init(Z_DIR_PIN);
    gpio_set_dir(Z_DIR_PIN, GPIO_IN);

    gpio_init(A_DIR_PIN);
    gpio_set_dir(A_DIR_PIN, GPIO_IN);

    gpio_init(B_DIR_PIN);
    gpio_set_dir(B_DIR_PIN, GPIO_IN);

    gpio_init(C_DIR_PIN);
    gpio_set_dir(C_DIR_PIN, GPIO_IN);

    // Set the pulse pins as INPUTS which trigger an INTERRUPT.
    //gpio_init(X_STEP_PIN);
    gpio_set_irq_enabled(X_STEP_PIN, GPIO_IRQ_EDGE_RISE, true);

    //gpio_init(Y_STEP_PIN);
    gpio_set_irq_enabled(Y_STEP_PIN, GPIO_IRQ_EDGE_RISE, true);

    //gpio_init(Z_STEP_PIN);
    gpio_set_irq_enabled(Z_STEP_PIN, GPIO_IRQ_EDGE_RISE, true);

    //gpio_init(A_STEP_PIN);
    gpio_set_irq_enabled(A_STEP_PIN, GPIO_IRQ_EDGE_RISE, true);

    //gpio_init(B_STEP_PIN);
    gpio_set_irq_enabled(B_STEP_PIN, GPIO_IRQ_EDGE_RISE, true);

    //gpio_init(C_STEP_PIN);
    gpio_set_irq_enabled(C_STEP_PIN, GPIO_IRQ_EDGE_RISE, true);

    irq_set_priority(IO_IRQ_BANK0, 0x00);
    irq_set_exclusive_handler(IO_IRQ_BANK0, step_irq_handler);
    irq_set_enabled(IO_IRQ_BANK0, true);
}

/*
 * Main entry method of the application.
 */
int main() {
    stdio_init_all();

    multicore_launch_core1(core1Code);

    // Send the position of all three axis to the virtual USB Serialport.
    while (true) {
        printf("<Pos:%d,%d,%d,%d,%d,%d>\n", x_step_count, y_step_count, z_step_count, a_step_count, b_step_count, c_step_count);

        // Wait for 16 msec (±60 Hz).
        // Don't use sleep_ms, it will send the processor into low power mode, causing it to miss steps.
        busy_wait_us_32(1000 * 16);
    }

    return 0;
}
