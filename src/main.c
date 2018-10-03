#include "display.h"
#include <math.h>

//http://yann.lecun.com/exdb/mnist/

int main() {
	glfwInit();

	VkBool32 debug = 0;

#ifdef _DEBUG
	//debug = 1;
#endif

	VKLInstance* instance;
	vklCreateInstance(&instance, NULL, debug, NULL);

	VKLDevice* device;
	VKLDeviceGraphicsContext** deviceContexts;

	vklCreateDevice(instance, &device, NULL, 0, NULL, 1, &deviceContexts);

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
	
	pixels[0] = 'A';
	pixels[1] = 'C';
	pixels[2] = 'D';
	pixels[3] = 'F';

	char* shaderPaths[1];
	shaderPaths[0] = "res/basic-comp.spv";

	VkShaderStageFlagBits stages[1];
	stages[0] = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutBinding bindings[3];
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	bindings[0].pImmutableSamplers = NULL;

	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	bindings[1].pImmutableSamplers = NULL;

	bindings[2].binding = 2;
	bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	bindings[2].descriptorCount = 1;
	bindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
	bindings[2].pImmutableSamplers = NULL;

	VKLShaderCreateInfo shaderCreateInfo;
	memset(&shaderCreateInfo, 0, sizeof(VKLShaderCreateInfo));
	shaderCreateInfo.shaderPaths = shaderPaths;
	shaderCreateInfo.shaderStages = stages;
	shaderCreateInfo.shaderCount = 1;
	shaderCreateInfo.bindings = bindings;
	shaderCreateInfo.bindingsCount = 3;

	VKLShader* shader;
	vklCreateShader(device, &shader, &shaderCreateInfo);

	VKLPipeline* pipeline;
	vklCreateComputePipeline(device, &pipeline, shader);

	VkCommandBuffer cmdBuffer;
	vklAllocateCommandBuffer(devCon, &cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	VKLUniformObject* uniform;
	vklCreateUniformObject(device, &uniform, shader);

	char* pixelsOut = malloc_c(sizeof(char) * 4);

	VKLBuffer* inBuffer;
	vklCreateBuffer(device, &inBuffer, VK_FALSE, sizeof(char) * 4, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	
	vklSetUniformStorageBuffer(device, uniform, inBuffer, 0);

	VKLBuffer* outBuffer;
	vklCreateBuffer(device, &outBuffer, VK_FALSE, sizeof(char) * 4, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	vklSetUniformStorageBuffer(device, uniform, outBuffer, 1);

	VKLBuffer* infoBuffer;
	vklCreateBuffer(device, &infoBuffer, VK_FALSE, sizeof(int) * 2, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);

	vklSetUniformStorageBuffer(device, uniform, infoBuffer, 2);

	int imageInfo[] = {28, 28};

	vklWriteToMemory(device, infoBuffer->memory, imageInfo, sizeof(int) * 2);
	
	printf("%d %d %d %d\n", pixels[0], pixels[1], pixels[2], pixels[3]);
	printf("%d %d %d %d\n\n", pixelsOut[0], pixelsOut[1], pixelsOut[2], pixelsOut[3]);

	vklWriteToMemory(device, inBuffer->memory, pixels, sizeof(char) * 4);

	vklBeginCommandBuffer(device, cmdBuffer);

	device->pvkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->pipeline);

	device->pvkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE,
		pipeline->pipelineLayout, 0, 1, &uniform->descriptorSet, 0, 0);

	device->pvkCmdDispatch(cmdBuffer, 1, 1, 1);

	vklEndCommandBuffer(device, cmdBuffer);

	vklExecuteCommandBuffer(devCon, cmdBuffer);

	vklReadFromMemory(device, outBuffer->memory, pixelsOut, sizeof(char) * 4);

	printf("%d %d %d %d\n", pixels[0], pixels[1], pixels[2], pixels[3]);
	printf("%d %d %d %d\n\n", pixelsOut[0], pixelsOut[1], pixelsOut[2], pixelsOut[3]);

	//showImage(instance, header, pixels);

	vklDestroyInstance(instance);

	glfwTerminate();
	
	printf("Enter a key to exit:");
	getchar();
	return 0;
}
