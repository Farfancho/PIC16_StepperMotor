#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SERIAL_LINE_BUFFER_SIZE   64U
#define SERIAL_TX_BUFFER_SIZE     64U

void SerialInit(void);
//solo para buffers, banderas etc

void SerialRxISR(void);
                    //van dentro de la interrupcion
void SerialTxISR(void);

void SerialRequestPositionReportFromISR(int32_t position_steps);
//desde el timer1

bool SerialLineAvailable(void);

bool SerialReadLine(char *dst, uint8_t max_len);

bool SerialSendString(const char *text);

bool SerialIsTxBusy(void);

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_H */