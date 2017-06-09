#include <SPI.h>
#include <GD.h>

#include "image.h"

void setup()
{
  GD.begin();
  for (byte y = 0; y < 32; y++)
    GD.copy(RAM_PIC + y * 64, image_pic + y * 32, 32);
  GD.copy(RAM_CHR, image_chr, sizeof(image_chr));
  GD.copy(RAM_PAL, image_pal, sizeof(image_pal));
}

void loop()
{
}
