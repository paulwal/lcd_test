/*
 *  font.h
 *  Manage bitmap fonts.
*/

#ifndef FONT_H
#define FONT_H

/* Accessing Fonts */
unsigned char * font_glyph_bitmap   (short font, unsigned char glyph, unsigned char *width, unsigned char *page_height);
unsigned char * font_spacing_bitmap (short font, unsigned char *width, unsigned char *page_height);

/* Creating Fonts */
/* Stores the width and offset of individual glyphs in a font. */
struct Font_Glyphs {
    unsigned char  width;     // The width of the character in pixels.
    unsigned short offset;    // The offset of the character's bitmap, in bytes, into the font's Font_Bitmaps data array.
};

/* Stores the actual glyph bitmaps in a font. */
typedef unsigned char Font_Bitmaps;

/* Stores meta data about a font. */
struct Font {
    unsigned char              page_height;    // Height in pages (8 pixels) of the font's characters.
    unsigned char              start_char;     // The first character in the font (in glyphs and bitmaps).
    unsigned char              space_width;    // The width in pixels of a space character.
    unsigned char              spacing;        // The space between characters in pixels.
    const struct Font_Glyphs  *glyphs;         // Pointer to array of glyph information.
    const unsigned char       *bitmaps;        // Pointer to array of glyph bitmaps.
};

#endif