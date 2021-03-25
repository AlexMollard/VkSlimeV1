#include <assert.h>
#include <vulkan/vulkan.h>
#include <string>
#include "WindowApplication.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

struct Version
{
	Version(uint32_t major, uint32_t minor, uint32_t patch) : _major{ major }, _minor{ minor }, _patch{ patch } {}

	uint32_t _major = 0;
	uint32_t _minor = 0;
	uint32_t _patch = 0;
};

void CreateInstance(VkInstance& instance, const char* appName, const char* engineName, Version appVersion, Version engineVersion, Version apiVersion)
{
	const uint32_t extentionCount = 1;
	const char* instanceExtentions[extentionCount] = { "VK_EXT_debug_report" };
	const uint32_t layerCount = 1;
	const char* instanceLayers[extentionCount] = { "VK_LAYER_KHRONOS_validation" };

	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = appName;
	app_info.pEngineName = engineName;
	app_info.engineVersion = VK_MAKE_VERSION(engineVersion._major, engineVersion._minor, engineVersion._patch);
	app_info.applicationVersion = VK_MAKE_VERSION(appVersion._major, appVersion._minor, appVersion._patch);
	app_info.apiVersion = VK_MAKE_VERSION(apiVersion._major, apiVersion._minor, apiVersion._patch);

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;
	create_info.enabledExtensionCount = extentionCount;
	create_info.ppEnabledExtensionNames = instanceExtentions;
	create_info.enabledLayerCount = layerCount;
	create_info.ppEnabledLayerNames = instanceLayers;


	VkResult result = vkCreateInstance(
		&create_info,
		NULL,
		&instance
	);

	assert(result == VK_SUCCESS);
}

bool HasGraphicsExtensions(const VkPhysicalDevice& physicalDevice, const char** requiredExtensions, uint32_t extentionCount)
{
	uint32_t deviceExtentionCount = 0;
	vkEnumerateDeviceExtensionProperties(
		physicalDevice,
		nullptr,
		&deviceExtentionCount,
		nullptr
	);

	VkExtensionProperties* extensions = new VkExtensionProperties[deviceExtentionCount];
	vkEnumerateDeviceExtensionProperties(
		physicalDevice,
		nullptr,
		&deviceExtentionCount,
		extensions
	);

	for (uint32_t i = 0; i < extentionCount; i++)
	{
		bool extensionFound = false;
		for (uint32_t k = 0; k < deviceExtentionCount; k++)
		{
			if (strcmp(requiredExtensions[i], extensions[k].extensionName) == 0)
			{
				extensionFound = true;
				break;
			}
		}

		if (!extensionFound)
		{
			delete[] extensions;
			return false;
		}
	}
	delete[] extensions;

	return true;
}

bool GetQueueFamily(const VkPhysicalDevice& physicalDevice, VkQueueFlags flags, uint32_t queueFamilyIndex)
{
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		nullptr
	);

	VkQueueFamilyProperties* queueFamilies = new VkQueueFamilyProperties[queueFamilyCount];
	vkGetPhysicalDeviceQueueFamilyProperties(
		physicalDevice,
		&queueFamilyCount,
		queueFamilies
	);

	for (uint32_t i = 0; i < queueFamilyCount; i++)
	{
		if (queueFamilies[i].queueCount > 0)
		{
			if ((queueFamilies[i].queueFlags & flags) == flags)
			{
				queueFamilyIndex = i;
				delete[] queueFamilies;
				return true;
			}
		}
	}

	delete[] queueFamilies;
	return false;
}

int main(int argc, char **argv)
{
	// Creating Instance
	VkInstance instance = VK_NULL_HANDLE;
	CreateInstance(
		instance,				// Window Instance
		"VSlimeApp",			// App Name
		"VSlime",				// Engine Name
		Version(1, 0, 0),		// App Version
		Version(1, 0, 0),		// Engine Version
		Version(1, 2, 170)		// API Version
	);


	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(
		instance,
		&deviceCount,
		nullptr
	);
	
	VkPhysicalDevice* devices = new VkPhysicalDevice[deviceCount];
	vkEnumeratePhysicalDevices(
		instance,
		&deviceCount,
		devices
	);

	const uint32_t extensionCount = 1;
	const char* deviceExtensions[extensionCount] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	
	VkPhysicalDevice chosenDevice = VK_NULL_HANDLE;
	uint32_t chosenQueueFamilyIndex = 0;
	VkPhysicalDeviceProperties chosenPhysicalDeviceProperties;
	VkPhysicalDeviceFeatures chosenPhysicalDeviceFeatures;
	VkPhysicalDeviceMemoryProperties chosenPhysicalDeviceMemoryProperties;

	for (uint32_t i = 0; i < deviceCount; i++)
	{
		if (HasGraphicsExtensions(devices[i], deviceExtensions, extensionCount))
		{
			uint32_t queueFamilyIndex = 0;
			if (GetQueueFamily(devices[i], VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT, queueFamilyIndex))
			{
				VkPhysicalDeviceProperties physicalDeviceProperties;
				vkGetPhysicalDeviceProperties(
					devices[i],
					&physicalDeviceProperties
				);

				VkPhysicalDeviceFeatures physicalDeviceFeatures;
				vkGetPhysicalDeviceFeatures(
					devices[i],
					&physicalDeviceFeatures
				);

				VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
				vkGetPhysicalDeviceMemoryProperties(
					devices[i],
					&physicalDeviceMemoryProperties
				);

				if (chosenDevice == VK_NULL_HANDLE || physicalDeviceProperties.deviceType == VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					chosenDevice = devices[i];
					chosenPhysicalDeviceProperties = physicalDeviceProperties;
					chosenPhysicalDeviceFeatures = physicalDeviceFeatures;
					chosenPhysicalDeviceMemoryProperties = physicalDeviceMemoryProperties;
					chosenQueueFamilyIndex = queueFamilyIndex;
				}
			}
		}
	}

	assert(chosenDevice != VK_NULL_HANDLE);

	WindowApplication app{};

	try {
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
		vkDestroyInstance(instance, NULL);
	}

	return EXIT_SUCCESS;


	// Destroying Instance
	vkDestroyInstance(instance, NULL);

	return 0;
}