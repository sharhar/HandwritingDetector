#pragma once

#include <VKL/VKL.h>
#include <GLFW/glfw3.h>

typedef struct ImageDataHeader {
	int magicNumber;
	int imageNumber;
	int rowNumber;
	int columnNumber;
} ImageDataHeader;

void showImage(VKLInstance* instance, ImageDataHeader* header, char* pixels);
