# 🚀 Projeto: Interface de Comunicação Serial com RP2040

## 🎯 Objetivo

Este projeto tem como objetivo explorar e consolidar o uso de interfaces de comunicação serial no microcontrolador RP2040, utilizando a placa de desenvolvimento BitDogLab. As funcionalidades implementadas demonstram o domínio das tecnologias UART, I2C, manipulação de LEDs e interrupções com botões.

## ⚙️ Funcionalidades

#### 1. Modificação da Biblioteca font.h

Os caracteres escolhidos foram t-e-c, ficando #include "tec/font.h".

#### 2. Entrada de Caracteres via Serial (UART)

Utilizar o Serial Monitor do VS Code para enviar caracteres do tipo letra e número ao microcontrolador.

Exibir o caractere recebido no display SSD1306.

Caso o caractere seja um número (0 a 9), exibir um símbolo correspondente na matriz 5x5 de LEDs WS2812.

#### 3. Interação com o Botão A

Pressionar o botão A alterna o estado do LED RGB verde (ligado/desligado).

Exibir no display SSD1306 uma mensagem informando o estado do LED.

Enviar um texto descritivo para o Serial Monitor.

#### 4. Interação com o Botão B

Pressionar o botão B alterna o estado do LED RGB azul (ligado/desligado).

Exibir no display SSD1306 uma mensagem informando o estado do LED.

Enviar um texto descritivo para o Serial Monitor.

## 🎥 Link para Vídeo Demonstrativo
https://drive.google.com/file/d/1sHPEim84ud_yzAHf2XItK1k_GEdp3SxW/view?usp=sharing

## 📌 Considerações Finais

Este projeto representa uma oportunidade prática para consolidar conhecimentos em comunicação serial, controle de hardware e desenvolvimento embarcado. Ele demonstra a integração de diversos componentes e técnicas fundamentais para projetos com microcontroladores.
