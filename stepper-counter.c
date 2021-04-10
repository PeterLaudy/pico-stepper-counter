#include <stdio.h>
#include "pico/stdlib.h"
// #include "hardware/gpio.h"

const int MM_PER_STEP = (1000 / 40);

const uint X_DIR_PIN = 2;
const uint X_STEP_PIN = 5;
const uint Y_DIR_PIN = 1;
const uint Y_STEP_PIN = 4;
const uint Z_DIR_PIN = 0;
const uint Z_STEP_PIN = 3;
volatile int x_step_count = 0;
volatile int y_step_count = 0;
volatile int z_step_count = 0;

void step_irq_handler(uint gpio, uint32_t events) {
    if (gpio == X_STEP_PIN) {
        x_step_count += gpio_get(X_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }
    if (gpio == Y_STEP_PIN) {
        y_step_count += gpio_get(Y_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }
    if (gpio == Z_STEP_PIN) {
        z_step_count += gpio_get(Z_DIR_PIN) ? -MM_PER_STEP : +MM_PER_STEP;
    }
}

int main() {
    stdio_init_all();

    gpio_init(X_DIR_PIN);
    gpio_set_dir(X_DIR_PIN, GPIO_IN);

    gpio_init(Y_DIR_PIN);
    gpio_set_dir(Y_DIR_PIN, GPIO_IN);

    gpio_init(Z_DIR_PIN);
    gpio_set_dir(Z_DIR_PIN, GPIO_IN);

    gpio_init(X_STEP_PIN);
    gpio_set_irq_enabled_with_callback(X_STEP_PIN, GPIO_IRQ_EDGE_RISE, true, step_irq_handler);

    gpio_init(Y_STEP_PIN);
    gpio_set_irq_enabled_with_callback(Y_STEP_PIN, GPIO_IRQ_EDGE_RISE, true, step_irq_handler);

    gpio_init(Z_STEP_PIN);
    gpio_set_irq_enabled_with_callback(Z_STEP_PIN, GPIO_IRQ_EDGE_RISE, true, step_irq_handler);

    while (true) {
        printf("<Pos:%d,%d,%d>\n", x_step_count, y_step_count, z_step_count);
        busy_wait_us_32(1000);
    }

    return 0;
}
