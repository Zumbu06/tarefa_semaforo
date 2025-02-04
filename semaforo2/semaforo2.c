#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Definição dos pinos dos LEDs e do botão
#define LED_AZUL 11
#define LED_VERMELHO 12
#define LED_VERDE 13
#define BOTAO 5

// Estados do sistema
typedef enum {
    DESLIGADO,
    TODOS_LIGADOS,
    VERMELHO_E_VERDE,
    SOMENTE_VERDE
} EstadoSistema;

EstadoSistema estadoAtual = DESLIGADO;
bool sequenciaAtiva = false; // Controla se a sequência está em execução

// Função para desligar todos os LEDs
void desligarLEDs() {
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_VERDE, 0);
}

// Função de callback do alarme
int64_t callback_alarme(alarm_id_t id, void *user_data) {
    switch (estadoAtual) {
        case TODOS_LIGADOS:
            gpio_put(LED_AZUL, 0); // Desliga o LED azul
            estadoAtual = VERMELHO_E_VERDE;
            break;
        case VERMELHO_E_VERDE:
            gpio_put(LED_VERMELHO, 0); // Desliga o LED vermelho
            estadoAtual = SOMENTE_VERDE;
            break;
        case SOMENTE_VERDE:
            gpio_put(LED_VERDE, 0); // Desliga o LED verde
            estadoAtual = DESLIGADO;
            sequenciaAtiva = false; // Finaliza a sequência
            break;
        default:
            break;
    }

    // Se a sequência ainda não terminou, programa o próximo alarme
    if (estadoAtual != DESLIGADO) {
        add_alarm_in_ms(3000, callback_alarme, NULL, false);
    }

    return 0; // Retorno necessário para a função de callback
}

// Função para tratar o clique do botão
void tratarCliqueBotao() {
    if (!sequenciaAtiva) { // Só inicia a sequência se não estiver ativa
        sequenciaAtiva = true;
        estadoAtual = TODOS_LIGADOS;

        // Liga todos os LEDs
        gpio_put(LED_AZUL, 1);
        gpio_put(LED_VERMELHO, 1);
        gpio_put(LED_VERDE, 1);

        // Configura o primeiro alarme
        add_alarm_in_ms(3000, callback_alarme, NULL, false);
    }
}

// Função de debounce para o botão
bool debounceBotao() {
    static uint32_t ultimoTempo = 0;
    uint32_t tempoAtual = to_ms_since_boot(get_absolute_time());

    if (tempoAtual - ultimoTempo > 200) { // Debounce de 200ms
        ultimoTempo = tempoAtual;
        return true;
    }
    return false;
}

int main() {
    stdio_init_all();

    // Configuração dos pinos dos LEDs como saída
    gpio_init(LED_AZUL);
    gpio_init(LED_VERMELHO);
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    // Configuração do pino do botão como entrada
    gpio_init(BOTAO);
    gpio_set_dir(BOTAO, GPIO_IN);
    gpio_pull_up(BOTAO); // Habilita o resistor de pull-up interno

    // Inicializa todos os LEDs desligados
    desligarLEDs();

    // Loop principal
    while (true) {
        if (!gpio_get(BOTAO) && debounceBotao()) { // Verifica o clique do botão com debounce
            tratarCliqueBotao();
        }
        sleep_ms(10); // Pequeno delay para evitar leituras excessivas
    }

    return 0;
}