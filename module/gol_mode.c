#include "gol_mode.h"

// this
#include "globals.h"
#include "keyboard_helper.h"

// tables
#include "table.h"

// teletype
#include "teletype.h"
#include "teletype_io.h"

// libavr32
#include "font.h"
#include "region.h"
#include "util.h"

// asf
#include "conf_usb_host.h" // needed in order to include "usb_protocol_hid.h"
#include "usb_protocol_hid.h"

static bool dirty;

static uint8_t CursorVarX = 0;
static uint8_t CursorVarY = 0;
static int CursorTLine = 0;

static uint8_t GolVarX = 0;
static uint8_t GolVarY = 0;
//static uint8_t debug;

// teletype_io.h
void tele_gol_updated()//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
{
    dirty = true;
}

void set_gol_mode() //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
{
    dirty = true;
}

void gol_down()
{
    if (CursorVarY < 7)
    {
        CursorVarY++;
        GolVarY++;
       
    }
    else if (CursorTLine < 7)
    {
        CursorTLine++;
        CursorVarY = 0;
        GolVarY++;
        
    }
}

void gol_up()
{
    if (CursorVarY)
    {
        CursorVarY--;
        GolVarY--;
        
    }
    else if (CursorTLine)
    {
        CursorTLine--;
        CursorVarY=7;
        GolVarY--;
        
    }
    
}

void gol_left()
{
    if (CursorVarX)
    {
        CursorVarX--;
        GolVarX--;
       
    }
}

void gol_right()
{
    if (CursorVarX < 127)
    {
        CursorVarX++;
        GolVarX++;
        
    }
}



void process_gol_keys(uint8_t k, uint8_t m, bool is_held_key)
{
    // <down>: move down
    if (match_no_mod(m, k, HID_DOWN))
    {
        gol_down();
        dirty = true;
    }

    // <shift+down>
    else if (match_shift(m, k, HID_DOWN)) 
    {
        gol_down();
        dirty = true;
    }
    
    // <up>: move up
    else if (match_no_mod(m, k, HID_UP))
    {
        gol_up();
        dirty = true;
    }

    // <shift+up>
    else if (match_shift(m, k, HID_UP)) 
    {
        gol_up();
        dirty = true;
    }
    
    // <left>: move left
    else if (match_no_mod(m, k, HID_LEFT))
    {
        gol_left();
        dirty = true;
    }

    // <shift+left>
    else if (match_shift(m, k, HID_LEFT)) 
    {
        gol_left();
        dirty = true;
    }
    
    // <right>: move right
    else if (match_no_mod(m, k, HID_RIGHT))
    {
        gol_right();
        dirty = true;
    }

    // <shift+right>
    else if (match_shift(m, k, HID_RIGHT)) 
    {
        gol_right();
        dirty = true;
    }

    //<spacebar>
    else if (match_no_mod(m, k, HID_SPACEBAR)) 
    {
       gol_next_gen(&scene_state.gol_grid);
        
        dirty = true;
    }

    //<enter>
    else if (match_no_mod(m, k, HID_ENTER)) 
    {
        
        if (gol_isalive(&scene_state.gol_grid, GolVarX, GolVarY) == 1){
            gol_flip_off(&scene_state.gol_grid, GolVarX, GolVarY);
        }
        else {gol_flip_on(&scene_state.gol_grid, GolVarX, GolVarY);}
        dirty=true;
    }
}    
       


uint8_t screen_refresh_gol()
{


    if (!dirty)
    {
        return 0;
    }
    else
    {   
        //Clear BG and Cursor
        for (uint8_t y = 0; y < 8; y++) {//bgfill
            region_fill(&line[y], 0);
            region_fill_part(&line[CursorTLine], CursorVarX + CursorVarY*128, 1, 15);
            }
        //Render Alive Cells
        for (uint8_t i = 0; i < GOL_X; i++) {
            for (uint8_t o = 0; o < GOL_Y; o++) {
                if (gol_isalive(&scene_state.gol_grid, i, o) == 1) {
                    region_fill_part(&line[o/8], i+o*128-((o/8)*128*8), 1, 8);       
                }
            }
         }        
            
    
    dirty = false;

    return 0xFF;
    }
}
//debug func
//region_fill_part(&line[7], 0, debug, 15);