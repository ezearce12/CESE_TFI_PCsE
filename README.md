# Driver para pantalla OLED SSD1306 y sensor TF-LC02

Este repositorio contiene una implementación modular de drivers para:

- Pantalla OLED basada en el controlador SSD1306 (comunicación I2C)
- Sensor de distancia LiDAR TF-LC02 (comunicación UART con interrupciones)


## Funcionalidad

### SSD1306 (Display OLED)

- Inicialización de pantalla
- Impresión de texto mediante fuente 5x7
- Manejo de cursor
- Visualización de datos (distancia, estado, muestreo)
- Capa de puerto adaptada a HAL I2C de STM32

### TF-LC02 (Sensor LiDAR)

- Comunicación UART con interrupciones
- Envío de comandos y recepción de tramas
- Máquina de estados para parseo de datos
- Acceso a distancia medida, puertos y configuración


## Requisitos

- STM32 HAL (I2C y UART)
- Pantalla SSD1306 (128x64, I2C addr 0x3C)
- Sensor TF-LC02 (UART 115200 bps)
