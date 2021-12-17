#ifndef CLINT_H__
#define CLINT_H__

#define D1_MSIP0        0x14000000
#define D1_MTIMECMPL0   0x14004000
#define D1_MTIMECMPH0   0x14004004
#define D1_MTIME        0x1400BFF8

#define D1_SSIP0        0x1400C000
#define D1_STIMECMPL0   0x1400D000
#define D1_STIMECMPH0   0x1400D004

void clint_timer_cmp_set_val(int val);
void clint_soft_irq_clear(void);
void clint_soft_irq_init(void);

#define CLINT                    (0x14000000L)
#define CLINT_MTIMECMPL(hartid)  (CLINT + 0x4000 + 4*(hartid))
#define CLINT_MTIMECMPH(hartid)  (CLINT + 0x4004 + 4*(hartid))

#endif
