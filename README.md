# Controlador de motor a pasos con PIC16F18426 usando NCO y comunicación serial

## Descripción
Este proyecto implementa un controlador embebido para un motor a pasos basado en el microcontrolador **PIC16F18426**. El sistema genera la señal **STEP** usando el periférico **NCO (Numerically Controlled Oscillator)**, gestiona la dirección del motor desde firmware y recibe comandos por **puerto serial** para ejecutar movimientos de posición.

El desarrollo nace en el contexto de una práctica de laboratorio orientada al diseño de un actuador lineal o rotacional controlado por comandos estándar, con reporte de posición en tiempo real.

## Objetivo del proyecto
Construir un sistema de control de movimiento que permita:

- mover un motor a pasos a una posición objetivo,
- configurar parámetros de movimiento por comunicación serial,
- enviar la posición actual en tiempo real,
- servir como base para un actuador rotacional o translacional de alta precisión.

## Contexto académico
La práctica asociada plantea, entre otros requisitos:

- control del motor a pasos por medio de un microcontrolador,
- recepción de comandos por puerto serial,
- configuración de perfil de velocidad, velocidad máxima y resolución de micro-pasos,
- implementación de comandos tipo G-code,
- envío de la posición en tiempo real a **10 Hz**.

## Arquitectura general
El sistema se divide en varios bloques funcionales:

### 1. Control de movimiento
Responsable de:

- almacenar el estado actual del motor,
- gestionar posición actual y posición objetivo,
- definir sentido de giro,
- iniciar o detener el movimiento,
- actualizar la frecuencia de pasos.

### 2. Generación de pulsos STEP con NCO
La salida STEP se genera con el periférico **NCO1** del PIC16F18426. Esto permite:

- producir una frecuencia precisa sin cargar continuamente la CPU,
- ajustar dinámicamente la velocidad del motor modificando el valor de incremento,
- trabajar con modos de salida **FDC** o **PFM** según la estrategia de control.

### 3. Comunicación serial
La interfaz UART permite:

- recibir comandos desde un PC,
- configurar el comportamiento del sistema,
- solicitar o transmitir el estado actual,
- reportar la posición periódicamente.

### 4. Temporización e interrupciones
Se usan interrupciones para separar tareas en tiempo real, por ejemplo:

- actualización de la posición,
- transmisión periódica de telemetría,
- atención de recepción serial,
- sincronización del movimiento.

## Posible estructura del repositorio
```text
.
├── README.md
├── docs/
│   ├── practica-2.pdf
│   ├── pic16f18426-datasheet.pdf
│   └── notas-nco.md
├── firmware/
│   ├── src/
│   │   ├── main.c
│   │   ├── motor.c
│   │   ├── serial.c
│   │   ├── commands.c
│   │   ├── nco_control.c
│   │   └── timer_tasks.c
│   ├── inc/
│   │   ├── motor.h
│   │   ├── serial.h
│   │   ├── commands.h
│   │   ├── nco_control.h
│   │   └── config.h
│   └── mcc_generated_files/
├── hardware/
│   ├── esquematico.pdf
│   ├── conexion-driver-stepper.png
│   └── pinout.md
└── examples/
    ├── comandos-serial.txt
    └── pruebas-posicionamiento.txt
```

## Hardware base
### Microcontrolador
- **PIC16F18426**
- Frecuencia de operación de hasta **32 MHz**
- Periféricos útiles para este proyecto:
  - **1 NCO**
  - **1 EUSART**
  - múltiples timers
  - **PPS** para remapeo de periféricos

### Etapa de potencia
Este proyecto asume un driver externo para motor a pasos, por ejemplo:

- DRV8825
- A4988
- TB6600

Con señales mínimas:

- `STEP`
- `DIR`
- `ENABLE` (opcional)

## Funcionamiento esperado
1. El usuario envía un comando por puerto serial.
2. El firmware interpreta el comando y actualiza la referencia de movimiento.
3. El NCO genera la frecuencia de pulsos correspondiente a la velocidad deseada.
4. El sistema actualiza la posición estimada del motor.
5. Un temporizador dispara el envío periódico de la posición actual.

## Comandos esperados
Esto depende de la implementación final, pero una interfaz mínima podría incluir:

```text
G0 X90        ; mover a 90 grados o a una posición absoluta
G0 X-20       ; mover a una posición relativa si así se define
M201 V500     ; fijar velocidad máxima
M202 R16      ; fijar resolución o microstepping
M114          ; reportar posición actual
M112          ; parada de emergencia
```

## Estado actual del desarrollo
Actualmente el proyecto está enfocado en:

- comprensión del periférico NCO,
- cálculo de frecuencia de pasos,
- estructuración del firmware en módulos,
- control de movimiento por posición,
- transmisión periódica de la posición por UART.

## Consideraciones de diseño
- El **NCO** permite generar pulsos de STEP con buena resolución y menor carga de CPU.
- La posición reportada puede ser **estimada** a partir de los pasos generados, por lo que depende de no perder pasos mecánicamente.
- La arquitectura modular facilita migrar entre distintos drivers o incluso entre sistemas rotacionales y translacionales.
- Para una versión más robusta, puede añadirse:
  - homing,
  - finales de carrera,
  - aceleración/desaceleración,
  - cola de comandos,
  - validación de comandos G-code.

## Compilación
Este proyecto está pensado para **MPLAB X + XC8**, con apoyo de **MCC Melody/Classic** para inicialización de periféricos.

Pasos típicos:

1. Abrir el proyecto en MPLAB X.
2. Verificar configuración del PIC objetivo.
3. Revisar la inicialización de:
   - reloj,
   - NCO1,
   - EUSART,
   - Timer1 u otro temporizador periódico,
   - PPS.
4. Compilar con XC8.
5. Programar el microcontrolador.

## Próximas mejoras
- implementar aceleración trapezoidal,
- separar parser de comandos del control de movimiento,
- soportar cola de instrucciones,
- documentar protocolo serial,
- agregar pruebas de precisión y repetibilidad,
- documentar la parte mecánica del actuador.

## Referencias
- Hoja de datos del **PIC16F18426/46**
- Documento de práctica de motores a pasos
- Notas sobre configuración del **NCO (Numerically Controlled Oscillator)**

## Autor
Completar con tu nombre, curso, universidad y fecha.
