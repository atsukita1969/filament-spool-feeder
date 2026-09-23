/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 atsukita1969
 */
// simavr harness: Arduino Pro Mini (ATmega328P 16MHz) + TMC2130 SPI model
// scenario file lines: <t_ms> <D4|VM> <0|1> ; last line: <t_ms> END 0
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "sim_avr.h"
#include "sim_elf.h"
#include "avr_ioport.h"
#include "avr_spi.h"

static avr_t *avr;
static double now_s(void){ return (double)avr->cycle / 16000000.0; }

/* ---- TMC2130 model ---- */
static uint32_t regs[128];
static int powered = 0;
static int gstat = 0;           // bit0 reset
static int cs = 1, bidx = 0;
static uint8_t rx[8];
static uint8_t out[5];
static uint32_t next_out = 0;
static int nwrites = 0;
static FILE *wlog;

static uint32_t read_reg(uint8_t a){
    if(!powered) return 0;
    switch(a){
    case 0x01: { uint32_t v = gstat; gstat = 0; return v; }
    case 0x04: return 0x11000000UL | 0x00000040UL;       // IOIN: VERSION=0x11
    case 0x6F: return 0x80000000UL;                      // DRV_STATUS: stst
    default: return regs[a];
    }
}
static void cs_changed(struct avr_irq_t *irq, uint32_t v, void *p){
    int nv = v ? 1 : 0;
    if(cs == 1 && nv == 0){ // frame start
        bidx = 0;
        out[0] = powered ? (uint8_t)(gstat & 1) : 0;
        out[1] = (next_out >> 24) & 0xFF; out[2] = (next_out >> 16) & 0xFF;
        out[3] = (next_out >> 8) & 0xFF;  out[4] = next_out & 0xFF;
    } else if(cs == 0 && nv == 1){ // frame end
        if(bidx >= 5){
            uint8_t a = rx[0] & 0x7F;
            uint32_t d = ((uint32_t)rx[1]<<24)|((uint32_t)rx[2]<<16)|((uint32_t)rx[3]<<8)|rx[4];
            if(rx[0] & 0x80){
                if(powered){
                    if(a == 0x01) gstat &= ~d; else regs[a] = d;
                    nwrites++;
                    if(wlog) fprintf(wlog, "%.4f W %02X %08X\n", now_s(), a, d);
                }
                next_out = powered ? regs[a] : 0;
            } else {
                next_out = read_reg(a);
                if(wlog) fprintf(wlog, "%.4f R %02X -> (next) %08X\n", now_s(), a, next_out);
            }
        }
    }
    cs = nv;
}
static void spi_out(struct avr_irq_t *irq, uint32_t v, void *p){
    uint8_t reply = 0;
    if(cs == 0){ if(bidx < 8) rx[bidx] = (uint8_t)v; reply = (bidx < 5) ? out[bidx] : 0; bidx++; }
    avr_raise_irq(avr_io_getirq(avr, AVR_IOCTL_SPI_GETIRQ(0), SPI_IRQ_INPUT), reply);
}
static void vm_set(int on){
    if(on && !powered){ memset(regs, 0, sizeof regs); gstat = 1; next_out = 0; powered = 1; }
    else if(!on){ powered = 0; }
}

/* ---- motor pins ---- */
static int en = 1;              // EN (PD7) level, LOW = enabled
static long steps_total = 0, steps_effective = 0;
static double first_step = -1, last_step = -1;
#define NB 4000
static long bins[NB];           // 50 ms bins of effective steps
static int step_level = 0;
static int driver_active(void){
    uint32_t ch = regs[0x6C];
    uint32_t ii = regs[0x10];
    return powered && en == 0 && (ch & 0x0F) != 0 && ((ii >> 8) & 0x1F) > 0;
}
static void step_cb(struct avr_irq_t *irq, uint32_t v, void *p){
    int nv = v ? 1 : 0;
    if(nv && !step_level){
        double t = now_s();
        steps_total++;
        if(driver_active()){
            steps_effective++;
            int b = (int)(t / 0.05); if(b >= 0 && b < NB) bins[b]++;
            if(first_step < 0) first_step = t;
            last_step = t;
        }
    }
    step_level = nv;
}
static int led = 0;
static void led_cb(struct avr_irq_t *irq, uint32_t v, void *p){
    int nv = v ? 1 : 0;
    if(nv != led && wlog) fprintf(wlog, "%.4f LED=%d\n", now_s(), nv);
    led = nv;
}
static void en_cb(struct avr_irq_t *irq, uint32_t v, void *p){
    int nv = v ? 1 : 0;
    if(nv != en && wlog) fprintf(wlog, "%.4f EN=%d\n", now_s(), nv);
    en = nv;
}

int main(int argc, char **argv){
    if(argc < 3){ fprintf(stderr, "usage: sim328 fw.elf scenario [writelog]\n"); return 1; }
    elf_firmware_t fw; memset(&fw, 0, sizeof fw);
    if(elf_read_firmware(argv[1], &fw)){ fprintf(stderr, "elf read fail\n"); return 1; }
    avr = avr_make_mcu_by_name("atmega328p");
    if(!avr){ fprintf(stderr, "mcu fail\n"); return 1; }
    avr_init(avr); avr->frequency = 16000000; avr->log = 0;
    avr_load_firmware(avr, &fw);
    wlog = (argc > 3) ? fopen(argv[3], "w") : NULL;

    avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 2), cs_changed, NULL);
    avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_SPI_GETIRQ(0), SPI_IRQ_OUTPUT), spi_out, NULL);
    avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 6), step_cb, NULL);
    avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 7), en_cb, NULL);
    avr_irq_register_notify(avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 0), led_cb, NULL);
    avr_irq_t *d4 = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('D'), 4);

    FILE *sc = fopen(argv[2], "r"); if(!sc){ perror("scenario"); return 1; }
    double ev_t[256]; char ev_n[256][8]; int ev_v[256]; int nev = 0;
    while(nev < 256 && fscanf(sc, "%lf %7s %d", &ev_t[nev], ev_n[nev], &ev_v[nev]) == 3) nev++;
    fclose(sc);
    int ei = 0; double tend = ev_t[nev-1] / 1000.0;
    avr_raise_irq(d4, 0);
    while(1){
        double t = now_s();
        while(ei < nev && t >= ev_t[ei] / 1000.0){
            if(!strcmp(ev_n[ei], "D4")) avr_raise_irq(d4, ev_v[ei]);
            else if(!strcmp(ev_n[ei], "VM")) vm_set(ev_v[ei]);
            else if(!strcmp(ev_n[ei], "ERR")) { if(powered) gstat |= 2; }
            ei++;
        }
        if(t >= tend) break;
        int st = avr_run(avr);
        if(st == cpu_Done || st == cpu_Crashed){ fprintf(stderr, "cpu state %d at %.3f\n", st, t); break; }
    }
    printf("steps_total=%ld steps_effective=%ld first=%.3f last=%.3f writes=%d\n", steps_total, steps_effective, first_step, last_step, nwrites);
    printf("final GCONF=%08X IHOLD_IRUN=%08X TPOWERDOWN=%08X CHOPCONF=%08X COOLCONF=%08X PWMCONF=%08X TPWMTHRS=%08X EN=%d powered=%d\n",
        regs[0x00], regs[0x10], regs[0x11], regs[0x6C], regs[0x6D], regs[0x70], regs[0x13], en, powered);
    printf("bins(50ms,steps/s):");
    int lastb = (int)(tend / 0.05);
    for(int b = 0; b <= lastb && b < NB; b++) if(bins[b]) printf(" %.2f:%ld", b*0.05, bins[b]*20);
    printf("\n");
    if(wlog) fclose(wlog);
    return 0;
}
