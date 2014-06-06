/*
 *  lcd.h
 *  Control an LCD.
 *
 *  LCD             : ERC24064-1
 *  Microcontroller : AVR ATmega
 *  Interface       : 8-bit parallel, 8080
 */

#ifndef LCD_H
#define LCD_H

void lcd_power_down     (void);
void lcd_initialize     (void);
void lcd_clear_area     (unsigned char column1, unsigned char page1, unsigned char column2, unsigned char page2);
void lcd_draw_string    (unsigned char column, unsigned char page, short font, char * string);
void lcd_draw_bitmap    (unsigned char column, unsigned char page, unsigned char * bitmap, unsigned char width, unsigned char pages);
void lcd_set_column     (unsigned char column);
void lcd_set_page       (unsigned char page);
void lcd_command        (unsigned char command);
void lcd_write          (unsigned char data);
unsigned char lcd_read  (void);


#endif