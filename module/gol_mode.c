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
static uint8_t CursorTLine = 0;
static int8_t TrCellTracker = 0;
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
    if (CursorVarY < 6)
    {
        CursorVarY += 2;
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
        CursorVarY -= 2;
        GolVarY--;
        
    }
    else if (CursorTLine)
    {
        CursorTLine--;
        CursorVarY = 6;
        GolVarY--;
        
    }
    
}

void gol_left()
{
    if (CursorVarX)
    {
        CursorVarX -= 2;
        GolVarX--;
       
    }
}

void gol_right()
{
    if (GolVarX < 63)
    {
        CursorVarX += 2;
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
       gol_next_gen(&scene_state);
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

    //<0-9>
    else if (no_mod(m) && k >= HID_1 && k <= HID_0)
    {
        if (k != HID_9)//filter out 9
        {
            uint8_t n = (k - HID_1 + 1) % 10; // convert HID numbers
            for (size_t i = 0; i < GOL_TR_CELLS; i++)
            {
                if (GolVarX == scene_state.trigcells[i].x 
                && GolVarY == scene_state.trigcells[i].y)
                {
                    TrCellTracker = i;
                    break;
                }
            }
            if (TrCellTracker >= GOL_TR_CELLS)
            {
                TrCellTracker = 0;
                gol_set_tr(&scene_state, GolVarX, GolVarY, TrCellTracker, n);
                TrCellTracker++;
            }
            else
            {
                gol_set_tr(&scene_state, GolVarX, GolVarY, TrCellTracker, n);
                TrCellTracker++;
            }
        }
        dirty = true;
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
        //1.Clear BG
        for (uint8_t y = 0; y < 8; y++) {//bgfill
            region_fill(&line[y], 0);
            }
        //2.Render Trig Cells
        for (uint8_t i = 0; i < GOL_TR_CELLS; i++)
        {
            if (scene_state.trigcells[i].script > 0) // render Trig Cells
            {
                uint8_t x = scene_state.trigcells[i].x;
                uint8_t y = scene_state.trigcells[i].y;
                uint8_t yy = y - (y / 4) * 4;
                region_fill_part(&line[y / 4], (x * 2) + (yy * 2 * 128), 2, 4);
                region_fill_part(&line[y / 4], (x * 2) + (yy * 2 * 128) + 128, 2, 4);
            }
        }
        //3.Render Alive Cells
            for (uint8_t i = 0; i < GOL_X; i++)
            {
                for (uint8_t j = 0; j < GOL_Y; j++)
                {
                    if (gol_isalive(&scene_state.gol_grid, i, j) == 1)
                    {
                        uint8_t y = j - (j / 4) * 4;
                        region_fill_part(&line[j / 4], (i * 2) + (y * 2 * 128), 2, 8);
                        region_fill_part(&line[j / 4], (i * 2) + (y * 2 * 128) + 128, 2, 8);
                    }
                    
                }
            }
            region_fill_part(&line[CursorTLine], CursorVarX + CursorVarY * 128, 2, 15);
            region_fill_part(&line[CursorTLine], CursorVarX + (CursorVarY + 1) * 128, 2, 12);

            dirty = false;
            return 0xFF;
    }
}