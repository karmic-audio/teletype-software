#ifndef _GOL_MODE_H_ //<<<<<<<<<
#define _GOL_MODE_H_ //<<<<<<<<<

#include <stdbool.h>
#include <stdint.h>

void set_gol_mode(void);//<<<<<<<<<
void process_gol_keys(uint8_t key, uint8_t mod_key, bool is_held_key);
uint8_t screen_refresh_gol(void);
void gol_up(void);
void gol_down(void);
void gol_left(void);
void gol_right(void);
#endif