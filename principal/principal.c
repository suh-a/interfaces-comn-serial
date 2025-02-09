#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "principal.pio.h"
#include "tec/ssd1306.h"
#include "tec/font.h"

// Definições dos pinos 
#define LED_GREEN       11
#define LED_BLUE        12
#define BUTTON_A        5
#define BUTTON_B        6
#define MATRIZ_LEDS     7
#define NUM_PIXELS      25
#define I2C_PORT        i2c1
#define DISPLAY_SDA     14
#define DISPLAY_SCL     15
#define DISPLAY_ADDR    0x3C

// Definição UART
#define UART_ID         uart0
#define BAUD_RATE       115200
#define UART_TX_PIN     0
#define UART_RX_PIN     1

// Definição de tempo
#define DEBOUNCE_DELAY_MS   50

// Variáveis globais
static volatile int current_number = 0;
static volatile uint32_t last_button_time = 0;
static volatile bool green_led_active = false;
static volatile bool blue_led_active = false;

static volatile bool update_green_display = false;
static volatile bool update_blue_display = false;

// Protótipo das funções
void setup_gpio(void);
uint32_t matrix_rgb(double r, double g, double b);
void matriz_number(PIO pio, uint sm, int number, double r, double g, double b);
bool debounce_check(void);
void gpio_callback(uint gpio, uint32_t events);

// Representação dos números
static const double number_patterns[10][25] = {
    {1,1,1,1,1, 1,0,0,0,1, 1,0,0,0,1, 1,0,0,0,1, 1,1,1,1,1},
    {0,1,1,1,0, 0,0,1,0,0, 0,0,1,0,0, 0,1,1,0,0, 0,0,1,0,0},
    {1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1},
    {1,1,1,1,1, 0,0,0,0,1, 0,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1},
    {0,1,0,0,0, 0,0,0,1,0, 0,1,1,1,1, 1,0,0,1,0, 0,1,0,0,1},
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1},
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,0, 1,1,1,1,1},
    {0,0,0,1,0, 0,0,1,0,0, 0,1,0,0,0, 0,0,0,0,1, 1,1,1,1,1},
    {1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1},
    {1,1,1,1,1, 0,0,0,0,1, 1,1,1,1,1, 1,0,0,0,1, 1,1,1,1,1}
};

// Inicializações
void setup_gpio(void) {
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    gpio_init(LED_BLUE);
    gpio_set_dir(LED_BLUE, GPIO_OUT);
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_init(BUTTON_B);
    gpio_set_dir(BUTTON_B, GPIO_IN);
    gpio_pull_up(BUTTON_B);
    gpio_set_irq_enabled_with_callback(BUTTON_A, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
    gpio_set_irq_enabled_with_callback(BUTTON_B, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
}

uint32_t matrix_rgb(double r, double g, double b) {
    unsigned char R = r * 255;
    unsigned char G = g * 255;
    unsigned char B = b * 255;
    return (G << 24) | (R << 16) | (B << 8);
}

void matriz_number(PIO pio, uint sm, int number, double r, double g, double b) {
    if (number < 0 || number > 9) return;
    double brightness = 0.3;
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t led_value = matrix_rgb(
            brightness * r * number_patterns[number][i],
            brightness * g * number_patterns[number][i],
            brightness * b * number_patterns[number][i]
        );
        pio_sm_put_blocking(pio, sm, led_value);
    }
}

bool debounce_check(void) {
    uint32_t current_time = to_ms_since_boot(get_absolute_time());
    if (current_time - last_button_time < DEBOUNCE_DELAY_MS) {
        return false;
    }
    last_button_time = current_time;
    return true;
}

void gpio_callback(uint gpio, uint32_t events) {

    if (!debounce_check()) return;
    if (gpio == BUTTON_A) {
        green_led_active = !green_led_active;
        gpio_put(LED_GREEN, green_led_active ? 1 : 0);
        update_green_display = true;
        printf("Botão A pressionado! LED Verde %s\n", green_led_active ? "ligado" : "desligado");
    } else if (gpio == BUTTON_B) {
        blue_led_active = !blue_led_active;
        gpio_put(LED_BLUE, blue_led_active ? 1 : 0);
        update_blue_display = true;
        printf("Botão B pressionado! LED Azul %s\n", blue_led_active ? "ligado" : "desligado");
    }
}

int main() {

    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    stdio_init_all();
    setvbuf(stdout, NULL, _IONBF, 0);
    setup_gpio();

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &principal_program);
    uint sm = pio_claim_unused_sm(pio, true);
    principal_program_init(pio, sm, offset, MATRIZ_LEDS);

    green_led_active = false;
    blue_led_active = false;

    gpio_put(LED_GREEN, 0);
    gpio_put(LED_BLUE, 0);
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(DISPLAY_SDA, GPIO_FUNC_I2C);
    gpio_set_function(DISPLAY_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(DISPLAY_SDA);
    gpio_pull_up(DISPLAY_SCL);
    ssd1306_t ssd;
    ssd1306_init(&ssd, 128, 64, false, DISPLAY_ADDR, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    printf("Insira um caractere (letra A-Z ou número 0-9):\n");

    while (true) {

        if (update_green_display || update_blue_display) {
            ssd1306_fill(&ssd, false);
            ssd1306_draw_string(&ssd, green_led_active ? "LED Verde ON" : "LED Verde OFF", 0, 0);
            ssd1306_draw_string(&ssd, blue_led_active ? "LED Azul ON" : "LED Azul OFF", 0, 10);
            ssd1306_send_data(&ssd);
            update_green_display = false;
            update_blue_display = false;
        }
        int c = getchar_timeout_us(10000);
        if (c != PICO_ERROR_TIMEOUT) {
            char car = (char)c;
            ssd1306_fill(&ssd, false);
            char buf[2] = {car, '\0'};
            if (car >= '0' && car <= '9') {
                current_number = car - '0';
                printf("Número digitado: %d\n", current_number);
                ssd1306_draw_string(&ssd, buf, 0, 0);
                ssd1306_send_data(&ssd);
                matriz_number(pio, sm, current_number, 0.0, 1.0, 1.0);
            } else if ((car >= 'A' && car <= 'Z') || (car >= 'a' && car <= 'z')) {
                printf("Letra digitada: %c\n", car);
                ssd1306_draw_string(&ssd, buf, 0, 0);
                ssd1306_send_data(&ssd);
            } else {
                printf("Caractere inválido.\n");
            }
            printf("Insira um caractere (letra A-Z ou número 0-9):\n");
        }
        sleep_ms(10);
    }
    return 0;
}
