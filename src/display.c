#include "display.h"

void showImage(VKLInstance* instance, ImageDataHeader* header, char* pixels) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Image", NULL, NULL);
	
	VKLSurface* surface;
	vklCreateGLFWSurface(instance, &surface, window);
	
	VKLDevice* device;
	VKLDeviceGraphicsContext** deviceContexts;
	vklCreateDevice(instance, &device, &surface, 1, &deviceContexts, 0, NULL);
	
	VKLDeviceGraphicsContext* devCon = deviceContexts[0];
	
	VKLSwapChain* swapChain;
	VKLFrameBuffer* backBuffer;
	vklCreateSwapChain(devCon, &swapChain, VK_TRUE);
	vklGetBackBuffer(swapChain, &backBuffer);

	float data[] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f,  0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 1.0f, 0.0f,

		-0.5f, -0.5f, 0.0f, 0.0f,
		0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f,
	};
	
	VKLBuffer* buffer;
	vklCreateStagedBuffer(devCon, &buffer, data, sizeof(float) * 6 * 4, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	
	char* shaderPaths[2];
	shaderPaths[0] = "res/basic-vert.spv";
	shaderPaths[1] = "res/basic-frag.spv";

	VkShaderStageFlagBits stages[2];
	stages[0] = VK_SHADER_STAGE_VERTEX_BIT;
	stages[1] = VK_SHADER_STAGE_FRAGMENT_BIT;

	size_t offsets[2] = { 0, sizeof(float) * 2 };
	VkFormat formats[2] = { VK_FORMAT_R32G32_SFLOAT, VK_FORMAT_R32G32_SFLOAT };

	VkDescriptorSetLayoutBinding bindings[2];
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = NULL;

	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[1].pImmutableSamplers = NULL;

	VKLShaderCreateInfo shaderCreateInfo;
	memset(&shaderCreateInfo, 0, sizeof(VKLShaderCreateInfo));
	shaderCreateInfo.shaderPaths = shaderPaths;
	shaderCreateInfo.shaderStages = stages;
	shaderCreateInfo.shaderCount = 2;
	shaderCreateInfo.bindings = bindings;
	shaderCreateInfo.bindingsCount = 2;
	shaderCreateInfo.vertexInputAttributeStride = sizeof(float) * 4;
	shaderCreateInfo.vertexInputAttributesCount = 2;
	shaderCreateInfo.vertexInputAttributeOffsets = offsets;
	shaderCreateInfo.vertexInputAttributeFormats = formats;

	VKLShader* shader;
	vklCreateShader(device, &shader, &shaderCreateInfo);

	VKLPipelineCreateInfo pipelineCreateInfo;
	memset(&pipelineCreateInfo, 0, sizeof(VKLPipelineCreateInfo));
	pipelineCreateInfo.shader = shader;
	pipelineCreateInfo.renderPass = backBuffer->renderPass;
	pipelineCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	pipelineCreateInfo.cullMode = VK_CULL_MODE_NONE;
	pipelineCreateInfo.extent.width = swapChain->width;
	pipelineCreateInfo.extent.height = swapChain->height;

	VKLGraphicsPipeline* pipeline;
	vklCreateGraphicsPipeline(device, &pipeline, &pipelineCreateInfo);

	VkCommandBuffer cmdBuffer;
	vklAllocateCommandBuffer(devCon, &cmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	VKLUniformObject* uniform;
	vklCreateUniformObject(device, &uniform, shader);

	float proj[] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};

	VKLBuffer* uniformBuffer;
	vklCreateBuffer(device, &uniformBuffer, VK_FALSE, sizeof(float) * 16, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

	vklSetUniformBuffer(device, uniform, uniformBuffer, 0);

	//float imageData[] = {
	//	1.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,
	//	1.0f, 0.3f, 0.3f,    0.3f, 1.0f, 0.3f,   0.3f, 0.3f, 1.0f,    0.3f, 0.3f, 0.3f,
	//	1.0f, 0.5f, 0.5f,    0.5f, 1.0f, 0.5f,   0.5f, 0.5f, 1.0f,    0.5f, 0.5f, 0.5f,
	//	1.0f, 0.8f, 0.8f,    0.8f, 1.0f, 0.8f,   0.8f, 0.8f, 1.0f,    0.8f, 0.8f, 0.8f
	//};


	char imageData[] = {
		255, 0, 255, 0,
		0, 255, 0, 255,
		255, 0, 255, 0,
		0, 255, 0, 255
	};

	VKLTextureCreateInfo textureCreateInfo;
	textureCreateInfo.width = header->rowNumber;
	textureCreateInfo.height = header->columnNumber;
	textureCreateInfo.format = VK_FORMAT_R8_UNORM;
	textureCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
	textureCreateInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
	textureCreateInfo.filter = VK_FILTER_NEAREST;
	textureCreateInfo.colorSize = sizeof(char);
	textureCreateInfo.colorCount = 1;

	VKLTexture* texture;
	vklCreateStagedTexture(devCon, &texture, &textureCreateInfo, pixels + (28*28*17));

	vklSetUniformTexture(device, uniform, texture, 1);

	vklSetClearColor(backBuffer, 0.25f, 0.45f, 1.0f, 1.0f);
	
	{ //Record rendering commands
		vklBeginCommandBuffer(device, cmdBuffer);

		vklBeginRender(device, backBuffer, cmdBuffer);

		VkViewport viewport = { 0, 0, swapChain->width, swapChain->height, 0, 1 };
		VkRect2D scissor = { 0, 0, swapChain->width, swapChain->height };
		VkDeviceSize vertexOffsets = 0;

		device->pvkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		device->pvkCmdSetScissor(cmdBuffer, 0, 1, &scissor);

		device->pvkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->pipeline);
		device->pvkCmdBindVertexBuffers(cmdBuffer, 0, 1, &buffer->buffer, &vertexOffsets);

		device->pvkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipeline->pipelineLayout, 0, 1, &uniform->descriptorSet, 0, NULL);

		device->pvkCmdDraw(cmdBuffer, 6, 1, 0, 0);

		vklEndRender(device, backBuffer, cmdBuffer);

		vklEndCommandBuffer(device, cmdBuffer);
	}
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		proj[12] = glfwGetTime() / 10.0f;

		vklWriteToMemory(device, uniformBuffer->memory, proj, sizeof(float) * 16);

		vklExecuteCommandBuffer(devCon, cmdBuffer);

		vklPresent(swapChain);
	}

	vklDestroyTexture(device, texture);
	vklDestroyUniformObject(device, uniform);
	vklDestroyGraphicsPipeline(device, pipeline);
	vklDestroyShader(device, shader);
	vklDestroyBuffer(device, uniformBuffer);
	vklDestroyBuffer(device, buffer);
	vklDestroySwapChain(swapChain);
	vklDestroyDevice(device);
	vklDestroySurface(instance, surface);
}