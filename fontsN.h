/* vim: set ai et ts=4 sw=4: */
#ifndef __FONTS_H__
#define __FONTS_H__

#include <stdint.h>

typedef struct {
    const uint8_t width;
    uint8_t height;
    flash uint16_t *data;
} FontDef;

extern FontDef Font_11x18;

#endif // __FONTS_H__
