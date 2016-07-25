/*
 * ringmem.h
 *
 * Created: 2014-09-03 17:48:41
 *  Author: Ioannis Glaropoulos
 *
 * Implementation of a circular FIFO buffer in Contiki OS.
 * The sole purpose of this API is to be used by device drivers,
 * that feed the CPU with data frames. Thus, allocation of new
 * packets shall normally occur inside interrupt context, while
 */ 


#ifndef RINGMEM_H_
#define RINGMEM_H_

#include "contiki-conf.h"



#define RINGMEM_CONCAT2(s1, s2) s1##s2
#define RINGMEM_CONCAT(s1, s2) RINGMEM_CONCAT2(s1, s2)

#define RINGMEM_NOM_MAXSIZE 16383
#ifndef RINGMEM_CONF_MAXSIZE
#define RINGMEM_MAXSIZE    RINGMEM_NOM_MAXSIZE
#else
#define RINGMEM_MAXSIZE    RINGMEM_CONF_MAXSIZE
#endif

/**
 * \brief    Structure of a Ring memory in Contiki OS.
 */
struct ringmem {
  uint8_t *data;      /* Points to the array of the data buffer */
  uint16_t size;      /* Buffer size does not need to be a power of two */
  uint16_t a_len;     /* The available capacity of the buffer */
  uint16_t put_ptr;   /* Points to the buffer feed position */
  uint16_t get_ptr;   /* Points to the buffer consume position*/
  uint16_t wrap_ptr;
};

/**
 * \brief    Structure of a Ring memory block in Contiki OS.
 *           The structure appears to have a fixed size, but
 *           the data can be of variable length and shall be
 *           stored in the tail of the structure.
 */
#pragma pack(1)
typedef struct ringmem_block {
  uint16_t len;
  const uint8_t data_ptr;
} ringmem_block_t;
#pragma pack()

/** \brief    Declare a ring FIFO memory. */
#define RINGMEM(name, len) \
  static struct ringmem RINGMEM_CONCAT(name, _ringmem) = { \
	  .size = len \
  }; \
  static struct ringmem *name = &(RINGMEM_CONCAT(name, _ringmem)); \
  static __attribute__((__aligned__(1))) uint8_t (RINGMEM_CONCAT(name, _buf))[len]


#define RINGMEM_INIT(name) \
  ringmem_init(name, &(RINGMEM_CONCAT(name, _buf)[0]), (name)->size)

/**
 * \brief     Initialize a ring FIFO memory.
 * \param r   A pointer to a struct ringmem to hold the state of the ring memory
 * \param a   A pointer to an array to hold the data in the ring memory
 * \param len The nominal size of this ring memory.
 */
void    ringmem_init(struct ringmem *r, uint8_t *a, uint16_t len);




/**
 * \brief     Allocate and copy a new block of memory inside the FIFO buffer.
 * \param r   A pointer to a struct ringmem to hold the state of the ring memory
 * \param len The size of data to be allocated and copied.
 *
 * This method shall normally be called inside interrupt context. If called in
 * user context, it must be atomic.
 */
int    ringmem_alloc(struct ringmem *r, uint8_t *data, uint16_t len);


/**
 * \brief     Get a pointer to the next memory block inside the FIFO buffer.
 * \param r   A pointer to a struct ringmem to hold the state of the ring memory
 * \return    A pointer to a struct ringmem_block to access the memory block
 *
 * This method shall normally be called in user context and thus it must be atomic.
 */
ringmem_block_t*     ringmem_get_next_block_ptr(struct ringmem *r);


/**
 * \brief     Free the first block of memory in the FIFO buffer.
 * \param r   A pointer to a struct ringmem to hold the state of the ring memory
 *
 * This method shall normally be called in user context and thus it must be atomic.
 */
void ringmem_free_block(struct ringmem *r);

#endif /* RINGMEM_H_ */