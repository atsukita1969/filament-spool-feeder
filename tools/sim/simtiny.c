/* SPDX-License-Identifier: GPL-3.0-or-later
 * Copyright (C) 2026 atsukita1969
 */
// simavr harness: ATtiny85 @8MHz jam monitor. Drives PB1 (request), watches PB3 (relay drive).
// scenario lines: <t_ms> REQ <0|1> ; last line <t_ms> END 0
// relay ON  <=> DDRB3=1 && PORTB3=1  (R3 pulls EXT_IN low otherwise)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sim_avr.h"
#include "sim_elf.h"
#include "avr_ioport.h"

static avr_t *avr;
int main(int argc, char **argv){
    if(argc < 3){ fprintf(stderr,"usage\n"); return 1; }
    elf_firmware_t fw; memset(&fw,0,sizeof fw);
    if(elf_read_firmware(argv[1], &fw)) return 1;
    avr = avr_make_mcu_by_name("attiny85"); avr_init(avr); avr->frequency = 8000000; avr->log = 0;
    avr_load_firmware(avr, &fw);
    avr_irq_t *pb1 = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 1);
    avr_irq_t *pb4 = avr_io_getirq(avr, AVR_IOCTL_IOPORT_GETIRQ('B'), 4);
    FILE *sc = fopen(argv[2],"r"); double et[20000]; char en[20000][8]; int ev[20000]; int n=0;
    while(n<20000 && fscanf(sc,"%lf %7s %d",&et[n],en[n],&ev[n])==3) n++;
    fclose(sc);
    int ei=0; double tend=et[n-1]/1000.0; int relay=-1; long cnt=0;
    int waddr = (argc>3)? (int)strtol(argv[3],0,16) : -1; double probe_t=-1; long misses=0, releases=0;
    const uint8_t DDRB_A = 0x37, PORTB_A = 0x38;
    avr_raise_irq(pb1, 0);
    avr_raise_irq(pb4, 0);
    while(1){
        double t = (double)avr->cycle/8000000.0;
        while(ei<n && t >= et[ei]/1000.0){ if(!strcmp(en[ei],"EXT")) avr_raise_irq(pb4, ev[ei]);
            else if(!strcmp(en[ei],"REQ")){ avr_raise_irq(pb1, ev[ei]); if(!ev[ei]) probe_t = t + 0.2; } ei++; }
        if(waddr>=0 && probe_t>0 && t>=probe_t){ int v = avr->data[waddr] | (avr->data[waddr+1]<<8); releases++; if(v) misses++; if(argc>4) printf("%.3f release: counter=%d\n", t, v); probe_t=-1; }
        if(t >= tend) break;
        int st = avr_run(avr);
        if(st==cpu_Done||st==cpu_Crashed){ fprintf(stderr,"cpu state %d at %.3f\n",st,t); break; }
        if((++cnt & 63)==0){
            int r = ((avr->data[DDRB_A]>>3)&1) && ((avr->data[PORTB_A]>>3)&1);
            int ddr = (avr->data[DDRB_A]>>3)&1;
            if(r != relay){ printf("%.4f relay=%s (DDRB3=%d)\n", t, r?"ON":"OFF", ddr); relay = r; }
        }
    }
    printf("end %.3f relay=%s releases=%ld not_reset=%ld\n", tend, relay?"ON":"OFF", releases, misses);
    return 0;
}
