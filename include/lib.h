#ifndef LIB_H
#define LIB_H

#define DMA_ENABLE 0x80000000
#define DMA_16 0x00000000

void memcpy16_dma(unsigned short* dest, unsigned short* source, int amount);

#endif
