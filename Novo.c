#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "inc/font.h"
#include "novaMatrix.pio.h" //biblioteca para controlar a matriz de leds

// Definições de I2C
// Este exemplo usará o I2C0 nos pinos GPIO8 (SDA) e GPIO9 (SCL) operando a 400KHz.
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C // Endereço do display SSD1306
#define L (0.03) // Luminosidade dos leds
#define buton_A 5
#define buton_B 6
#define LEDG 11
#define LEDB 12
#define LEDR 13
#define OUT_PIN 7   // Pino de dados conectado à matriz
#define NUM_LEDS 25 // Número de LEDs na matriz
static volatile uint32_t last_time = 0; // Variável para debounce do botão A
static volatile uint32_t last_time2 = 0; // Variável para debounce do botão B

// UART defines
#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 4 // Pino de transmissão UART
#define UART_RX_PIN 5 // Pino de recepção UART

ssd1306_t ssd; //Estrutura para controle do display OLED

// Matrizes para representar números na matriz de LEDs
double numero0[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero1[25] = {0.0, L, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};

double numero2[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero3[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

double numero4[25] = {0.0, L, 0.0, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, 0.0, L, 0.0};

double numero5[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};

double numero6[25] = {0.0, L, L, L, 0.0,
                      0.0, 0.0, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero7[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0,
                      0.0, 0.0, L, 0.0, 0.0};

double numero8[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0};

double numero9[25] = {0.0, L, L, L, 0.0,
                      0.0, L, 0.0, L, 0.0,
                      0.0, L, L, L, 0.0,
                      0.0, L, 0.0, 0.0, 0.0,
                      0.0, L, L, L, 0.0};
double limpa[25] = {0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0,
                    0.0, 0.0, 0.0, 0.0, 0.0};

// Converte valores RGB para um formato de 32 bits
uint32_t matrix_rgb(float r, float g, float b)
{
    unsigned char R, G, B;
    R = r * 255;
    G = g * 255;
    B = b * 255;

    return (G << 24) | (R << 16) | (B << 8);
}

// Protótipos das funções auxiliares
void initialize_gpio(); // Inicializa os pinos GPIO
void acendeRGB(bool r, bool g, bool b); // Acende os LEDs RGB
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b); // Exibe padrão na matriz
void numero(char letra);// Mostra um número na matriz de LEDs
bool ledON = false; //variável necessária para o código na função callback
bool ledONb = false; //variável necessária para o código na função callback
void button_callback(uint gpio, uint32_t events);// Callback para tratamento de interrupção dos botões

int main()
{
    // Inicialização do sistema
    PIO pio = pio0;
    bool ok;
    uint16_t i = 0;
    uint32_t valor_led;
    double r = 0.0, b = 0.0, g = 0.0;
    ok = set_sys_clock_khz(128000, false);
    printf("iniciando a transmissão PIO");
    if (ok)
        printf("clock set to %id\n", clock_get_hz(clk_sys));
    stdio_init_all();
    initialize_gpio();
    uint sm = pio_claim_unused_sm(pio, true);
    uint offset = pio_add_program(pio, &novaMatrix_program);
    novaMatrix_program_init(pio, sm, offset, OUT_PIN);

    // Configuração do I2C
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    // Inicializa a estrutura do display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT); // Inicializa o display
    ssd1306_config(&ssd);                                         // Configura o display
    ssd1306_send_data(&ssd);                                      // Envia os dados para o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    bool cor = true;

    // Configuração da UART
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Configuração das interrupções dos botões
    gpio_set_irq_enabled_with_callback(buton_A, GPIO_IRQ_EDGE_FALL, true, &button_callback);
    gpio_set_irq_enabled_with_callback(buton_B, GPIO_IRQ_EDGE_FALL, true, &button_callback);

    char letra; //Variável para armazenar informação fornecida para exibição no display

    while (true)
    {
        scanf("%c", &letra);
        printf("Letra digitada: %c\n", letra);
        ssd1306_fill(&ssd, 0);
        ssd1306_draw_char(&ssd, letra, 60, 20);
        ssd1306_send_data(&ssd);
        switch (letra)
        {
        case '0': padrao1(numero0, valor_led, pio, sm, r, g, b); break;
        case '1': padrao1(numero1, valor_led, pio, sm, r, g, b); break;
        case '2': padrao1(numero2, valor_led, pio, sm, r, g, b); break;
        case '3': padrao1(numero3, valor_led, pio, sm, r, g, b); break;
        case '4': padrao1(numero4, valor_led, pio, sm, r, g, b); break;
        case '5': padrao1(numero5, valor_led, pio, sm, r, g, b); break;
        case '6': padrao1(numero6, valor_led, pio, sm, r, g, b); break;
        case '7': padrao1(numero7, valor_led, pio, sm, r, g, b); break;
        case '8': padrao1(numero8, valor_led, pio, sm, r, g, b); break;
        case '9': padrao1(numero9, valor_led, pio, sm, r, g, b); break;
        default: padrao1(limpa, valor_led, pio, sm, r, g, b);    break;
        }
        sleep_ms(100); 
    }
}
void padrao1(double *desenho, uint32_t valor_led, PIO pio, uint sm, double r, double g, double b)
{

    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
        valor_led = matrix_rgb(desenho[24 - i], desenho[24 - i], b = 0.0);
        pio_sm_put_blocking(pio, sm, valor_led);
    }
}
void initialize_gpio()
{
    gpio_init(buton_A);
    gpio_set_dir(buton_A, GPIO_IN);
    gpio_pull_up(buton_A);

    gpio_init(buton_B);
    gpio_set_dir(buton_B, GPIO_IN);
    gpio_pull_up(buton_B);

    gpio_init(LEDB);
    gpio_set_dir(LEDB, GPIO_OUT);

    gpio_init(LEDG);
    gpio_set_dir(LEDG, GPIO_OUT);

    gpio_init(LEDR);
    gpio_set_dir(LEDR, GPIO_OUT);
}
void acendeRGB(bool r, bool g, bool b)
{
    gpio_put(LEDR, r);
    gpio_put(LEDG, g);
    gpio_put(LEDB, b);
}
void button_callback(uint gpio, uint32_t events)
{
    uint32_t tempo_atual = to_us_since_boot(get_absolute_time());

    if (gpio == buton_A && tempo_atual - last_time > 200000)
    {
        last_time = tempo_atual;
        ledON = !ledON;
        acendeRGB(0, ledON, 0);
        printf("Led verde %s\n", ledON ? "ligado" : "desligado");
        ssd1306_fill(&ssd, 0);
        ssd1306_draw_string(&ssd, ledON ? "Verde acesa!" : "Verde apagada!", 4, 10);
        ssd1306_send_data(&ssd);
    }
    else if (gpio == buton_B && tempo_atual - last_time2 > 200000)
    {
        last_time2 = tempo_atual;
        ledONb = !ledONb;
        acendeRGB(0, 0, ledONb);
        printf("Led azul %s\n", ledONb ? "ligado" : "desligado");
        ssd1306_fill(&ssd, 0);
        ssd1306_draw_string(&ssd, ledONb ? "Azul acesa!" : "Azul apagada!", 4, 10);
        ssd1306_send_data(&ssd);
    }
}

