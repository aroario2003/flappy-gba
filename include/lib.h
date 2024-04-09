#ifndef LIB_H
#define LIB_H

#define DMA_ENABLE 0x80000000
#define DMA_16 0x00000000

volatile unsigned int* dma_source = (volatile unsigned int*) 0x40000D4;

volatile unsigned int* dma_destination = (volatile unsigned int*) 0x40000D8;

volatile unsigned int* dma_count = (volatile unsigned int*) 0x40000DC;

void memcpy16_dma(unsigned short* dest, unsigned short* source, int amount);

#endif
