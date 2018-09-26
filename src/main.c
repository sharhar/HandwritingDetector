#include "display.h"
#include <math.h>

//http://yann.lecun.com/exdb/mnist/

int main() {
	glfwInit();

	VkBool32 debug = 0;

#ifdef _DEBUG
	debug = 1;
#endif

	VKLInstance* instance;
	vklCreateInstance(&instance, NULL, debug, NULL);

	VKLDevice* device;
	VKLDeviceGraphicsContext** deviceContexts;

	printf("Hello\n");

	vklCreateDevice(instance, &device, NULL, 0, NULL, 1, &deviceContexts);

	printf("Hello2\n");

	VKLDeviceGraphicsContext* devCon = deviceContexts[0];

	size_t size;
	char* imageData = readFileFromPath("res/train-images.idx3-ubyte", &size);
	
	{
		char temp[4];
		for (int i = 0; i < 4; i++) {
			temp[3] = imageData[i * 4 + 0];
			temp[2] = imageData[i * 4 + 1];
			temp[1] = imageData[i * 4 + 2];
			temp[0] = imageData[i * 4 + 3];

			imageData[i * 4 + 0] = temp[0];
			imageData[i * 4 + 1] = temp[1];
			imageData[i * 4 + 2] = temp[2];
			imageData[i * 4 + 3] = temp[3];
		}
	}
	
	ImageDataHeader* header = (ImageDataHeader*)imageData;
	char* pixels = imageData + 16;



	//showImage(instance, header, pixels);

	vklDestroyInstance(instance);

	glfwTerminate();
	
	printf("Enter a key to exit:");
	getchar();
	return 0;
}
