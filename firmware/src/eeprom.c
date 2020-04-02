#include "eeprom.h"
#include "stm8s.h"

void eeprom_write(uint8_t *pdest, const uint8_t *pdata, uint8_t len){
  uint8_t i;
  FLASH_Unlock(FLASH_MEMTYPE_DATA);
  for(i=0;i<len;i++)
  {
    FLASH_ProgramByte((u32)pdest, *pdata);
    pdest++;
    pdata++;
    while (!(FLASH->IAPSR & (FLASH_IAPSR_EOP)));
  }
  FLASH_Lock(FLASH_MEMTYPE_DATA);
}

void eeprom_read(uint8_t *pdest, const uint8_t*psrc, uint8_t len){
  while(len--)
    *pdest++ = *psrc++;
}
