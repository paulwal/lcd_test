/*
 *  lcd.c
 *  Control an LCD.
 *
 *  LCD             : ERC24064-1
 *  Microcontroller : AVR ATmega
 *  Interface       : 8-bit parallel, 8080
 */

#include "lcd.h"
#include "font.h"
#include <avr/io.h>
#include <avr/cpufunc.h>
#include <util/delay.h>
#include <string.h>
#include <stdlib.h>

/* Port C has five control pins and port D has eight data pins. */
#define DATA       PORTD
#define CTRL       PORTC
#define DATA_DDR   DDRD
#define CTRL_DDR   DDRC
#define RST        PINC0   // Reset
#define CS         PINC1   // Chip Select
#define CD         PINC2   // Control Data
#define WR         PINC3   // Read/Write or Write signal, aka RW or WR.
#define RD         PINC4   // Enable or Read signal, aka EN or RD.


/* Clear a rectangular area, where the upper left corner is column1,page1 and the bottom right corner is column2,page2. */
void lcd_clear_area(unsigned char column1, unsigned char page1, unsigned char column2, unsigned char page2)
{
    unsigned char page, column;
    for (page=page1; page<=page2; page++) {
        lcd_set_page(page);
        lcd_set_column(column1);
        for (column=column1; column<=column2; column++) {
            lcd_write(0b00000000);
        }
    }
}

/* Draw the given string in the specified font at the specified column and page. */
void lcd_draw_string(unsigned char column, unsigned char page, short font, char * string)
{
    int size = strlen(string);
    unsigned char * bm;
    unsigned char width, pages;
    int i;
    for (i=0; i<size; i++) {
        /* Draw the character. */
        bm = font_glyph_bitmap(font, string[i], &width, &pages);
        lcd_draw_bitmap(column, page, bm, width, pages);
        free(bm);
        column += width;

        /* Draw the trailing character spacing. */
        bm = font_spacing_bitmap(font, &width, &pages);
        lcd_draw_bitmap(column, page, bm, width, pages);
        free(bm);
        column += width;
    }
}

/* Draw a bitmap at the specified column and page. */
void lcd_draw_bitmap(unsigned char column, unsigned char page, unsigned char * bitmap, unsigned char width, unsigned char pages)
{
    unsigned char p, c;
    short index;

    /* Draw each byte in the bitmap. */
    for (p=0; p<pages; p++) {
        /* Set the drawing page. */
        lcd_set_page(page+p);

        for (c=0; c<width; c++) {
            /* The column only needs to be set for the first byte of each page. */
            if (c == 0) {
                lcd_set_column(column);
            }
            
            /* Draw the byte at the calculated bitmap index. */
            index = p*width+c;
            lcd_write(bitmap[index]);
        }
    }
}

/* Set the current drawing column. (0-240) */
void lcd_set_column(unsigned char column)
{
    /* This is a two-command operation. */
    lcd_command(0b00010000 | (column>>4));    // Set the most significant bits.
    lcd_command(0b00001111 & column);         // Set the least significant bits.
}

/* Set the current drawing page. (0-8) */
void lcd_set_page(unsigned char page)
{
    lcd_command(0b10110000 | page);
}

/* Power down the LCD before turning off its power. This allows the LCD to drain its capacitors. */
void lcd_power_down (void)
{
    /* Send the System Reset command. */
    lcd_command(0b11100010);
    _delay_ms(5);
}

/* Initialize the LCD. */
void lcd_initialize (void)
{
    /* Reset all control registers to their default states. */
    _delay_ms(15);
    CTRL |=  (1<<RST);
    _delay_ms(15);
    CTRL &= ~(1<<RST);
    _delay_ms(15);
    CTRL |=  (1<<RST);
    _delay_ms(15);

    /* Send the System Reset command. */
    lcd_command(0b11100010);
    _delay_ms(1);
    
    /* Set the multiplex rate (96) and temperature compensation (-0.20%/C). */
    lcd_command(0b00100011);

    /* Set the LCD mapping control (MY=1; MX=0). */
    lcd_command(0b11001000);

    /* Set the LCD bias ratio (10.7). */
    lcd_command(0b11101000);

    /* Set the gain and contrast (Gain=1) (Contrast=7). */
    lcd_command(0b10000001);
    lcd_command(0b01000111);

    /* Set the power control (internal; 60nF < LCD < 90nF). */
    lcd_command(0b00101111);
    
    /* Set the start line (0: no scrolling) */
    lcd_command(0b01000000);

    /* Set a normal display (all-pixels-on off; inverse display off). */
    lcd_command(0b10100100);
    lcd_command(0b10100110);

    /* Set RAM address control (Column/page wraparound restart, increment +1). */
    lcd_command(0b10001001);

    /* Set the page address (0). */
    lcd_command(0b10110000);

    /* Set display enable. */
    lcd_command(0b10101111);
}

/* Send a command instruction to the LCD. */
void lcd_command (unsigned char command)
{
    DATA_DDR  = 0b11111111;
    CTRL_DDR |= 0b00011111;
    CTRL |=  (1<<RD);
    CTRL |=  (1<<CS);
    CTRL &= ~(1<<CD);
    DATA  =  command;
    CTRL &= ~(1<<WR);
    _NOP();
    CTRL |=  (1<<WR);
    CTRL &= ~(1<<CS);
}

/* Send drawing data to the LCD. The given byte will be drawn at the currently set column and page. */
void lcd_write (unsigned char data)
{
    DATA_DDR  = 0b11111111;
    CTRL |=  (1<<RD);
    CTRL |=  (1<<CS);
    CTRL |=  (1<<CD);
    DATA  =  data;
    CTRL &= ~(1<<WR);
    _NOP();
    CTRL |=  (1<<WR);
    CTRL &= ~(1<<CS);
}

/* Read drawn data from the LCD. The byte at the current column and page will be returned. */
/****** Doesn't work -- don't know why -- might have something to do with dummy read cycle requirement. *******/
unsigned char lcd_read (void)
{
    DATA_DDR  = 0b00000000;
    CTRL |=  (1<<WR);
    CTRL |=  (1<<CD);
    CTRL |=  (1<<CS);
    CTRL &= ~(1<<RD);
    _NOP();
    CTRL |=  (1<<RD);
    CTRL &= ~(1<<CS);
    _NOP();
    CTRL |=  (1<<CS);
    CTRL &= ~(1<<RD);

    unsigned char data = DATA;
    CTRL &= ~(1<<CS);
    return data;
}