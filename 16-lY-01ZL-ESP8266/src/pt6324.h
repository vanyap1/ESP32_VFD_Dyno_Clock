/*
 * pt6324.h
 *
 * Created: 29.05.2024 22:54:31
 * Author: Vanya
 */
#include <stdint.h>

#ifndef PT6324_H_
#define PT6324_H_

#define MAX_BRIGHTNESS 7

#ifdef __cplusplus
extern "C" {
#endif

void pt63Init(void);
void pt63SetBrightness(uint8_t brightness);
void writeCmd(uint8_t cmd);
void writeData(uint8_t *data, uint8_t len);
void vfd_write_byte(uint8_t data); // Ця функція більше не використовується, але оголошення не зашкодить.
void writeChars(uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif // PT6324_H_