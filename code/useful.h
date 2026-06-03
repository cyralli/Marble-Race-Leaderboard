#ifndef USE_H
#define USE_H

#include "raylib.h"

int findElement(const char *arr[], int n, const char *key)
{
    int i;
    for (i = 0; i < n; i++)
        if (arr[i] == key)
            return i;

    // If the key is not found
    return -1;
}

unsigned long grayscaleRGBA(unsigned long color) {
    unsigned char r = (color >> 24) & 0xFF;
    unsigned char g = (color >> 16) & 0xFF;
    unsigned char b = (color >> 8) & 0xFF;
    unsigned char a = (color & 0xFF); // Save the alpha
    
    unsigned char l = (unsigned char)((0.2126 * r) + (0.7152 * g) + (0.0722 * b));
    
    unsigned char contrastVal = (l >= 128) ? 0xFF : 0x00;

    return ((unsigned long)contrastVal << 24) | 
           ((unsigned long)contrastVal << 16) | 
           ((unsigned long)contrastVal << 8)  | 
           a;
}

Image CircleMask(Image image) {
    Image mask = GenImageColor(image.width, image.height, BLANK);
    
    ImageDrawCircle(&mask, image.width/2, image.height/2, image.width/2, WHITE);
    
    Color *basePix = LoadImageColors(image);
    Color *maskPix = LoadImageColors(mask);
    
    for (int i = 0; i < image.width * image.height; i++)
    {
        // Use mask's red channel as alpha (since mask is white/transparent)
        basePix[i].a = maskPix[i].a;
    }
    
    Image maskedImage = {
        .data = basePix,
        .width = image.width,
        .height = image.height,
        .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
        .mipmaps = 1
    };
    
    return maskedImage;
}

float easeInOutQuart(float x) { return x < 0.5 ? 8 * x * x * x * x : 1 - pow(-2 * x + 2, 4) / 2; }

#endif