//
// Created by lihw on 20/02/23.
//

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <cstring>
#include <iostream>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <array>
#include <utility>
#include <stdexcept>
#include <cassert>
#include <cmath>
#include <limits>
#include <unistd.h>
#include <fstream>

#define VK_CHECK(FN) \
	{ \
		VkResult r = FN; \
		if (r != VK_SUCCESS) \
        { \
			throw std::runtime_error(#FN " failed!"); \
		} \
	}
