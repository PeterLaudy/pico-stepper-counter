#include <stdio.h>
#include <pico/stdlib.h>

/*
 * The resolution of the stepper motors and spindle.
 * 40 steps per mm gives us a step of 1000/40 = 0.025 mm each.
 */
const int MM_PER_STEP = (1000 / 40);

/*
 * This pin-layout makes it easy to connect the board to an Arduino UNO running GRBL.
 */
const uint X_DIR_PIN = 2;
const uint X_STEP_PIN = 5;

const uint Y_DIR_PIN = 1;
const uint Y_STEP_PIN = 4;

const uint Z_DIR_PIN = 0;
const uint Z_STEP_PIN = 3;

/*
 * The counters for the three stepper motors.
 */
volatile int x_step_count = 0;
volatile int y_step_count = 0;
volatile int z_step_count = 0;

/*
 * The interrupt routine which is triggered by each pulse for any of the three stepper motors.
 */
void step_irq_handler(uint gpio, uint32_t events) {

    // Check if the stepper motor for the X axis was pulsed.
    if (gpio == X_STEP_PIN) {
        // Depending on the direction we increase or decrease the position.
        x_step_count += gpio_get(X_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    // Check if the stepper motor for the Y axis was pulsed.
    if (gpio == Y_STEP_PIN) {
        // Depending on the direction we increase or decrease the position.
        y_step_count += gpio_get(Y_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }

    // Check if the stepper motor for the XZ axis was pulsed.
    if (gpio == Z_STEP_PIN) {
        // Depending on the direction we increase or decrease the position.
        z_step_count += gpio_get(Z_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }
}

/*
 * Main entry method of the application.
 */
int main() {
    stdio_init_all();

    // Set the direction pins as INPUT.
    gpio_init(X_DIR_PIN);
    gpio_set_dir(X_DIR_PIN, GPIO_IN);

    gpio_init(Y_DIR_PIN);
    gpio_set_dir(Y_DIR_PIN, GPIO_IN);

    gpio_init(Z_DIR_PIN);
    gpio_set_dir(Z_DIR_PIN, GPIO_IN);

    // Set the pulse pins as INPUTS which trigger an INTERRUPT.
    gpio_init(X_STEP_PIN);
    gpio_set_irq_enabled_with_callback(X_STEP_PIN, GPIO_IRQ_EDGE_RISE, true, step_irq_handler);

    gpio_init(Y_STEP_PIN);
    gpio_set_irq_enabled_with_callback(Y_STEP_PIN, GPIO_IRQ_EDGE_RISE, true, step_irq_handler);

    gpio_init(Z_STEP_PIN);
    gpio_set_irq_enabled_with_callback(Z_STEP_PIN, GPIO_IRQ_EDGE_RISE, true, step_irq_handler);

    // Send the position of all three axis to the virtual USB Serialport.
    while (true) {
        printf("<Pos:%d,%d,%d>\n", x_step_count, y_step_count, z_step_count);

        // Wait for one msec. Don't usesleep_ms, because it will send the processor into low
        // power mode,causing it to miss steps.
        busy_wait_us_32(1000);
    }

    return 0;
}
