#ifndef __GPIO_H__
#define __GPIO_H__

extern void gpio_init(void);
extern void gpo_ctrl(unsigned char outPortNO,int outputStat);
extern int gpi_getstat(unsigned char inPortNO);
extern void gpio_del(void);

#endif

