/*
 *  hello_universe.c
 *  Initialize the LCD and display something.
 *
 *  LCD       : ERC24064-1
 *  Interface : 8-bit parallel, 8080
*/

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

int main (void)
{
    lcd_initialize();
    _delay_ms(1000);
    lcd_draw_string(5, 0, 0, "The lazy brown fox something something...");
    lcd_draw_string(5, 2, 1, "The New York Times");
    lcd_draw_string(5, 4, 2, "012345");
    _delay_ms(15000);
    lcd_clear_area(0, 0, 240, 8);
    lcd_power_down();

    return 0;
}
