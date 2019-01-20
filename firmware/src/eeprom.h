#ifndef _EEPROM_H_
#define _EEPROM_H_
#include "stm8s.h"

void eeprom_write(uint8_t *pdest, const uint8_t *pdata, uint8_t len);
void eeprom_read(uint8_t *pdest, const uint8_t*psrc, uint8_t len);

#endif
