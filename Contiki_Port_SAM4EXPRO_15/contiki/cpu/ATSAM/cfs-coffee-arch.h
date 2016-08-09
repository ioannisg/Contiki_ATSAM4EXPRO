/*
 * cfs_coffee_arch.h
 *
 * Created: 2016-08-02 23:48:35
 *  Author: Ioannis Glaropoulos <ioannisg@kth.se>
 */ 
#include "flash_efc.h"

#ifndef CFS_COFFEE_ARCH_H_
#define CFS_COFFEE_ARCH_H_


#define COFFEE_PAGE_SIZE		    IFLASH_PAGE_SIZE
#define COFFEE_PAGES              ((IFLASH_SIZE - (COFFEE_ADDRESS - IFLASH_ADDR)) / COFFEE_PAGE_SIZE)
#define COFFEE_START              (COFFEE_ADDRESS & ~(COFFEE_PAGE_SIZE-1))
#define COFFEE_SIZE               (COFFEE_PAGES*COFFEE_PAGE_SIZE)


/* These must agree with the parameters passed to makefsdata */
#define COFFEE_SECTOR_SIZE        IFLASH_PAGE_SIZE
#define COFFEE_NAME_LENGTH        20

/* These are used internally by the coffee file system */
#define COFFEE_MAX_OPEN_FILES     4
#define COFFEE_FD_SET_SIZE        8
#define COFFEE_DYN_SIZE           (COFFEE_PAGE_SIZE*1)
/* Micro logs are not needed for single page sectors */
#define COFFEE_MICRO_LOGS         0
#define COFFEE_LOG_TABLE_LIMIT    16    /* It doesnt' matter as */
#define COFFEE_LOG_SIZE           128   /* COFFEE_MICRO_LOGS is 0. */

#if COFFEE_PAGES <= 127
#define coffee_page_t int8_t
#elif COFFEE_PAGES <= 0x7FFF
#define coffee_page_t int16_t
#elif COFFEE_PAGES <= 0x7FFFFFFF
#define coffee_page_t int32_t
#else
#error "two many pages for coffee"
#endif

#define COFFEE_WRITE(from_buf, size, offset) flash_write((COFFEE_START) + offset, from_buf, size, 1) 

#define COFFEE_ERASE(sector) flash_erase(sector)

#define COFFEE_READ(to_buf, size, offset) flash_read((COFFEE_START) + offset, to_buf, size)

#define COFFEE_INIT() flash_init(FLASH_ACCESS_MODE_128, 6)

inline void flash_read(uint32_t address, void *data, uint32_t length) {
  uint8_t *pdata = (uint8_t *) address;
  ENERGEST_ON(ENERGEST_TYPE_FLASH_READ);
  memcpy((uint8_t*)data, address, length);
  ENERGEST_OFF(ENERGEST_TYPE_FLASH_READ);
}

inline void flash_erase(uint8_t sector) {

  uint32_t data = 0;
  uint32_t addr, end;
  addr = (COFFEE_START) + (sector) * COFFEE_SECTOR_SIZE;
  end = addr + COFFEE_SECTOR_SIZE;
  /* This prevents from accidental write to CIB. */
  if(!(addr >= COFFEE_START && end <= COFFEE_START + COFFEE_SIZE -1)) {
    return;
  }

  for(; addr < end; addr += 4) {
    flash_write(addr, &data, 4, 1);
  }
}

#endif /* CFS_COFFEE_ARCH_H_ */