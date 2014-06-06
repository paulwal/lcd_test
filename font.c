/*
 *  font.c
 *  Manage bitmap fonts.
*/

#include "font.h"
#include "ms_sans_serif.h"
#include "old_english_text_mt_11pt.h"
#include "impact_28pt.h"
#include <avr/pgmspace.h>
#include <stdlib.h>

const struct Font *Fonts[] = { &MS_Sans_Serif, &Old_English_Text_MT_11pt, &Impact_28pt };


/* Returns a string of bytes for the given font and sets the width and page_height. */
unsigned char * font_glyph_bitmap (short font, unsigned char glyph, unsigned char *width, unsigned char *page_height)
{
    unsigned short offset;
    short glyph_index, size, index;

    /* Set the page height for the caller. */
    *page_height = Fonts[font]->page_height;

    /* Handle space characters specially. */
    if ( glyph == ' ' ) {
        /* Set the glyph width for the caller. */
        *width = Fonts[font]->space_width;
    } else {
        /* Calculate the glyph index (ascii code for the glyph character minus the ascii code for the starting character). */
        glyph_index = glyph - Fonts[font]->start_char;

        /* Set the glyph width for the caller. */
        *width = pgm_read_byte(&(Fonts[font]->glyphs[glyph_index].width));

        /* Get the offset of the glyph's bitmap. */
        offset = pgm_read_word(&(Fonts[font]->glyphs[glyph_index].offset));
    }

    /* Allocate space on the heap for an array of bytes that visually represent the glyph */
    size = (*width) * (*page_height);
    unsigned char * bitmap = malloc(size);

    /* Populate the bitmap array and return a pointer to it. */
    for (index=0; index<size; index++) {
        if ( glyph == ' ' )
            bitmap[index] = 0b00000000;
        else
            bitmap[index] = pgm_read_byte(&(Fonts[font]->bitmaps[offset+index]));
    }

    return bitmap;
}


unsigned char * font_spacing_bitmap (short font, unsigned char *width, unsigned char *page_height)
{
    short size, index;

    /* Set the page height for the caller. */
    *page_height = Fonts[font]->page_height;

    /* Set the glyph width for the caller. */
    *width = Fonts[font]->spacing;

    /* Allocate space on the heap for an array of bytes that comprise the bitmap. */
    size = (*width) * (*page_height);
    unsigned char * bitmap = malloc(size);

    /* Populate the bitmap array and return a pointer to it. */
    for (index=0; index<size; index++) {
        bitmap[index] = 0b00000000;
    }

    return bitmap;
}

