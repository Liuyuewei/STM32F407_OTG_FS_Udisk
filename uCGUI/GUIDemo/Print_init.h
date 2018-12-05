#ifndef PRINT_INIT_H

#define PRINT_INIT_H

#define PRINT_ERROR          0x08
#define PRINT_PAPER_LACK     0x0C


extern unsigned char Print_rcv_buf[10];
extern unsigned char rcv_print_count;

void Print_inital(void);
void Print_test(void);


#endif