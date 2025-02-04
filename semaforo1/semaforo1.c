#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição dos pinos dos LEDs
#define LED_VERMELHO 11
#define LED_AMARELO 12
#define LED_VERDE 13

// Variável para controlar o estado do semáforo
typedef enum {
    VERMELHO,
    AMARELO,
    VERDE
} EstadoSemaforo;

EstadoSemaforo estadoAtual = VERMELHO;

bool callback_temporizador(struct repeating_timer *t) {
    switch (estadoAtual) {
        case VERMELHO:
            gpio_put(LED_VERMELHO, 0);  // Apaga o LED vermelho
            gpio_put(LED_AMARELO, 1);   // Acende o LED amarelo
            estadoAtual = AMARELO;
            break;
        case AMARELO:
            gpio_put(LED_AMARELO, 0);   // Apaga o LED amarelo
            gpio_put(LED_VERDE, 1);     // Acende o LED verde
            estadoAtual = VERDE;
            break;
        case VERDE:
            gpio_put(LED_VERDE, 0);     // Apaga o LED verde
            gpio_put(LED_VERMELHO, 1);  // Acende o LED vermelho
            estadoAtual = VERMELHO;
            break;
    }
    return true;
}

int main() {
    stdio_init_all();

    // Configuração dos pinos dos LEDs como saída
    gpio_init(LED_VERMELHO);
    gpio_init(LED_AMARELO);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    // Inicializa o semáforo com o LED vermelho aceso
    gpio_put(LED_VERMELHO, 1);
    gpio_put(LED_AMARELO, 0);
    gpio_put(LED_VERDE, 0);

    // Configuração do temporizador periódico
    struct repeating_timer temporizador;
    add_repeating_timer_ms(3000, callback_temporizador, NULL, &temporizador);

    // Loop principal
    while (true) {
        sleep_ms(1000);
    }

    return 0;
}