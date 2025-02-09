# Projeto: Controle de Matriz de LEDs com Raspberry Pi Pico

## Descrição
Este projeto utiliza um Raspberry Pi Pico para controlar uma matriz de LEDs RGB, um display OLED SSD1306 e interagir com botões para alternar cores dos LEDs. A comunicação com o display é feita via I2C, enquanto os LEDs RGB são acionados via GPIO.

## Recursos Principais
- Controle de uma matriz de 5x5 LEDs RGB.
- Exibição de números na matriz de LEDs.
- Utilização de um display OLED SSD1306.
- Interação via UART para entrada de caracteres.
- Uso de botões físicos para controle de LEDs RGB.

## Componentes Utilizados
- **Raspberry Pi Pico**
- **Matriz de LEDs RGB 5x5**
- **Display OLED SSD1306**
- **Botões de entrada**

## Configuração de Hardware
- **I2C**
  - SDA: GPIO14
  - SCL: GPIO15
- **UART**
  - TX: GPIO4
  - RX: GPIO5
- **Botões**
  - Botão A: GPIO5
  - Botão B: GPIO6
- **LEDs RGB**
  - Vermelho: GPIO13
  - Verde: GPIO11
  - Azul: GPIO12

## Como Executar
1. Configure o ambiente de desenvolvimento com o SDK do Raspberry Pi Pico.
2. Compile o código e envie para o Raspberry Pi Pico.
3. Conecte o hardware conforme a seção "Configuração de Hardware".
4. Interaja via UART ou botões para exibir diferentes padrões na matriz de LEDs.

## Exemplo de Uso
- Enviar um número via UART para exibição na matriz de LEDs.
- Pressionar o botão A para alternar o LED verde.
- Pressionar o botão B para alternar o LED azul.

## Demonstração
https://youtube.com/shorts/ogty3olQsh4

## Autor
DIEGO MICKAEL DE SÁ PEREIRA

