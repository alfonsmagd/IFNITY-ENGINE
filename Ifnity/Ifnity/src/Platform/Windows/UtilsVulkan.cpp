


#include "UtilsVulkan.h"
#include "Ifnity\Graphics\Utils.hpp"
#include <glslang\Public\resource_limits_c.h>


IFNITY_NAMESPACE



//-----------------------------------------------//
//STATIC UTILS METHODS INSIDE FILE IMPLEMENTATION//
//-----------------------------------------------//
static glslang_stage_t getGLSLangShaderStage(VkShaderStageFlagBits stage)
{
	switch(stage)
	{
	case VK_SHADER_STAGE_VERTEX_BIT:
		return GLSLANG_STAGE_VERTEX;
	case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
		return GLSLANG_STAGE_TESSCONTROL;
	case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
		return GLSLANG_STAGE_TESSEVALUATION;
	case VK_SHADER_STAGE_GEOMETRY_BIT:
		return GLSLANG_STAGE_GEOMETRY;
	case VK_SHADER_STAGE_FRAGMENT_BIT:
		return GLSLANG_STAGE_FRAGMENT;
	case VK_SHADER_STAGE_COMPUTE_BIT:
		return GLSLANG_STAGE_COMPUTE;
	case VK_SHADER_STAGE_TASK_BIT_EXT:
		return GLSLANG_STAGE_TASK;
	case VK_SHADER_STAGE_MESH_BIT_EXT:
		return GLSLANG_STAGE_MESH;

		// ray tracing
	case VK_SHADER_STAGE_RAYGEN_BIT_KHR:
		return GLSLANG_STAGE_RAYGEN;
	case VK_SHADER_STAGE_ANY_HIT_BIT_KHR:
		return GLSLANG_STAGE_ANYHIT;
	case VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR:
		return GLSLANG_STAGE_CLOSESTHIT;
	case VK_SHADER_STAGE_MISS_BIT_KHR:
		return GLSLANG_STAGE_MISS;
	case VK_SHADER_STAGE_INTERSECTION_BIT_KHR:
		return GLSLANG_STAGE_INTERSECT;
	case VK_SHADER_STAGE_CALLABLE_BIT_KHR:
		return GLSLANG_STAGE_CALLABLE;
	default:
		assert(false);
	};
	assert(false);
	return GLSLANG_STAGE_COUNT;
}



const char* getVulkanResultString(VkResult result)
{
	#define RESULT_CASE(res) \
  case res:              \
    return #res
	switch(result)
	{
		RESULT_CASE(VK_SUCCESS);
		RESULT_CASE(VK_NOT_READY);
		RESULT_CASE(VK_TIMEOUT);
		RESULT_CASE(VK_EVENT_SET);
		RESULT_CASE(VK_EVENT_RESET);
		RESULT_CASE(VK_INCOMPLETE);
		RESULT_CASE(VK_ERROR_OUT_OF_HOST_MEMORY);
		RESULT_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY);
		RESULT_CASE(VK_ERROR_INITIALIZATION_FAILED);
		RESULT_CASE(VK_ERROR_DEVICE_LOST);
		RESULT_CASE(VK_ERROR_MEMORY_MAP_FAILED);
		RESULT_CASE(VK_ERROR_LAYER_NOT_PRESENT);
		RESULT_CASE(VK_ERROR_EXTENSION_NOT_PRESENT);
		RESULT_CASE(VK_ERROR_FEATURE_NOT_PRESENT);
		RESULT_CASE(VK_ERROR_INCOMPATIBLE_DRIVER);
		RESULT_CASE(VK_ERROR_TOO_MANY_OBJECTS);
		RESULT_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED);
		RESULT_CASE(VK_ERROR_SURFACE_LOST_KHR);
		RESULT_CASE(VK_ERROR_OUT_OF_DATE_KHR);
		RESULT_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
		RESULT_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
		RESULT_CASE(VK_ERROR_VALIDATION_FAILED_EXT);
		RESULT_CASE(VK_ERROR_FRAGMENTED_POOL);
		RESULT_CASE(VK_ERROR_UNKNOWN);
		// Provided by VK_VERSION_1_1
		RESULT_CASE(VK_ERROR_OUT_OF_POOL_MEMORY);
		// Provided by VK_VERSION_1_1
		RESULT_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE);
		// Provided by VK_VERSION_1_2
		RESULT_CASE(VK_ERROR_FRAGMENTATION);
		// Provided by VK_VERSION_1_2
		RESULT_CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
		// Provided by VK_KHR_swapchain
		RESULT_CASE(VK_SUBOPTIMAL_KHR);
		// Provided by VK_NV_glsl_shader
		RESULT_CASE(VK_ERROR_INVALID_SHADER_NV);
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
		#endif
		#ifdef VK_ENABLE_BETA_EXTENSIONS
		// Provided by VK_KHR_video_queue
		RESULT_CASE(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
		#endif
		// Provided by VK_EXT_image_drm_format_modifier
		RESULT_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
		// Provided by VK_KHR_global_priority
		RESULT_CASE(VK_ERROR_NOT_PERMITTED_KHR);
		// Provided by VK_EXT_full_screen_exclusive
		RESULT_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_THREAD_IDLE_KHR);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_THREAD_DONE_KHR);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_OPERATION_DEFERRED_KHR);
		// Provided by VK_KHR_deferred_host_operations
		RESULT_CASE(VK_OPERATION_NOT_DEFERRED_KHR);
	default:
		return "Unknown VkResult Value";
	}
	#undef RESULT_CASE
}



PFN_vkSetDebugUtilsObjectNameEXT gvkSetDebugUtilsObjectNameEXT = nullptr;



IFNITY_INLINE VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsMessengerCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT             messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	// Select prefix depending on flags passed to the callback
	std::string prefix("");

	if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		prefix = "VERBOSE: ";
	}
	else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		prefix = "INFO: ";
	}
	else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		prefix = "WARNING: ";
	}
	else if(messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		prefix = "ERROR: ";
	}

	std::cerr << prefix << " validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

IFNITY_INLINE VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

IFNITY_INLINE void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if(func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}



//Set debug object name without instance and previously luaded vkSetDebugUtilsObjectNameEXT 
VkResult setDebugObjectName(VkDevice device, VkObjectType type, uint64_t handle, const char* name)
{
	if(!name || !*name)
	{
		return VK_SUCCESS;
	}
	const VkDebugUtilsObjectNameInfoEXT ni = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType = type,
		.objectHandle = handle,
		.pObjectName = name,
	};

	if(gvkSetDebugUtilsObjectNameEXT != nullptr)
	{
		return gvkSetDebugUtilsObjectNameEXT(device, &ni);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


//
bool setupDebugCallbacksVK123(VkInstance instance, VkDebugUtilsMessengerEXT* debugMessenger)
{
	#ifdef _DEBUG
	VkDebugUtilsMessengerCreateInfoEXT create_info;
	create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	create_info.pfnUserCallback = debugUtilsMessengerCallback;
	create_info.pNext = NULL;
	create_info.flags = 0;


	if(CreateDebugUtilsMessengerEXT(instance, &create_info, nullptr, debugMessenger) != VK_SUCCESS)
	{
		IFNITY_LOG(LogCore, ERROR, "Failed to create debug utils messenger in Vulkan Device");
		return false;

	}


	#endif

	return true;
}

VkResult setDebugObjectName(VkInstance instance, VkDevice device, VkObjectType type, uint64_t handle, const char* name)
{
	if(!name || !*name)
	{
		return VK_SUCCESS;
	}
	const VkDebugUtilsObjectNameInfoEXT ni = {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT,
		.objectType = type,
		.objectHandle = handle,
		.pObjectName = name,
	};

	auto func = (PFN_vkSetDebugUtilsObjectNameEXT)vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT");
	if(func != nullptr)
	{
		return func(device, &ni);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}


VkSemaphore createSemaphore(VkDevice device, const char* debugName)
{
	const VkSemaphoreCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		.flags = 0,
	};
	VkSemaphore semaphore = VK_NULL_HANDLE;
	VK_CHECK(vkCreateSemaphore(device, &ci, nullptr, &semaphore), "error create semaphore");
	VK_CHECK(setDebugObjectName(device, VK_OBJECT_TYPE_SEMAPHORE, (uint64_t)semaphore, debugName), "error set debug Object Name type Semaphore");
	return semaphore;
}


VkFence createFence(VkDevice device, const char* debugName)
{
	const VkFenceCreateInfo ci = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = 0,
	};
	VkFence fence = VK_NULL_HANDLE;
	VK_CHECK(vkCreateFence(device, &ci, nullptr, &fence), "error create fence ");
	VK_CHECK(setDebugObjectName(device, VK_OBJECT_TYPE_FENCE, (uint64_t)fence, debugName), "error debug objet name type fence");
	return fence;
}

void imageMemoryBarrier(VkCommandBuffer buffer,
	VkImage image,
	VkAccessFlags srcAccessMask,
	VkAccessFlags dstAccessMask,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask,
	VkImageSubresourceRange subresourceRange)
{
	const VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.srcAccessMask = srcAccessMask,
		.dstAccessMask = dstAccessMask,
		.oldLayout = oldImageLayout,
		.newLayout = newImageLayout,
		.image = image,
		.subresourceRange = subresourceRange,
	};
	vkCmdPipelineBarrier(buffer, srcStageMask, dstStageMask, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}


void saveSPIRVBinaryFile(const char* filename, const uint8_t* code, size_t size)
{
	FILE* f = fopen(filename, "wb");

	if(!f)
		return;

	fwrite(code, sizeof(uint8_t), size, f);
	fclose(f);
}

VkResult compileShaderVK(VkShaderStageFlagBits stage, const char* code, std::vector<uint8_t>* outSPIRV, const glslang_resource_t* glslLangResource)
{
	glslang_initialize_process();

	if(!outSPIRV)
	{
		IFNITY_LOG(LogCore, ERROR, "outSPIRV is null, check it out");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	const glslang_input_t input = {
		.language = GLSLANG_SOURCE_GLSL,
		.stage = getGLSLangShaderStage(stage),
		.client = GLSLANG_CLIENT_VULKAN,
		.client_version = GLSLANG_TARGET_VULKAN_1_3,
		.target_language = GLSLANG_TARGET_SPV,
		.target_language_version = GLSLANG_TARGET_SPV_1_6,
		.code = code,
		.default_version = 100,
		.default_profile = GLSLANG_NO_PROFILE,
		.force_default_version_and_profile = false,
		.forward_compatible = false,
		.messages = GLSLANG_MSG_DEFAULT_BIT,
		.resource = glslLangResource,
	};

	glslang_shader_t* shader = glslang_shader_create(&input);
	

	if(!glslang_shader_preprocess(shader, &input))
	{
		IFNITY_LOG(LogCore,TRACE,"Shader preprocessing failed:\n");
		IFNITY_LOG(LogCore,TRACE,"  %s\n", glslang_shader_get_info_log(shader));
		IFNITY_LOG(LogCore,TRACE,"  %s\n", glslang_shader_get_info_debug_log(shader));
		
		assert(false);
		IFNITY_LOG(LogCore, ERROR, "glslang_shader_preprocess() failed");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	if(!glslang_shader_parse(shader, &input))
	{
		IFNITY_LOG(LogCore,TRACE,"Shader parsing failed:\n");
		IFNITY_LOG(LogCore,TRACE,"  %s\n", glslang_shader_get_info_log(shader));
		IFNITY_LOG(LogCore,TRACE,"  %s\n", glslang_shader_get_info_debug_log(shader));
		Utils::printShaderSource(glslang_shader_get_preprocessed_code(shader));
		IFNITY_LOG(LogCore, ERROR, "glslang_shader_parse() failed");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	glslang_program_t* program = glslang_program_create();
	glslang_program_add_shader(program, shader);

	//SCOPE_EXIT{
	//  glslang_program_delete(program);
	//};

	if(!glslang_program_link(program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
	{
		IFNITY_LOG(LogCore,TRACE,"Shader linking failed:\n");
		IFNITY_LOG(LogCore,TRACE,"  %s\n", glslang_program_get_info_log(program));
		IFNITY_LOG(LogCore,TRACE,"  %s\n", glslang_program_get_info_debug_log(program));
		IFNITY_LOG(LogCore, ERROR, "glslang program link() failed");
		return VK_ERROR_INITIALIZATION_FAILED;
	}

	glslang_spv_options_t options = {
		.generate_debug_info = true,
		.strip_debug_info = false,
		.disable_optimizer = false,
		.optimize_size = true,
		.disassemble = false,
		.validate = true,
		.emit_nonsemantic_shader_debug_info = false,
		.emit_nonsemantic_shader_debug_source = false,
	};

	glslang_program_SPIRV_generate_with_options(program, input.stage, &options);

	if(glslang_program_SPIRV_get_messages(program))
	{
		IFNITY_LOG(LogCore,ERROR,"%s\n", glslang_program_SPIRV_get_messages(program));
	}

	const uint8_t* spirv = reinterpret_cast<const uint8_t*>(glslang_program_SPIRV_get_ptr(program));
	const size_t numBytes = glslang_program_SPIRV_get_size(program) * sizeof(uint32_t);

	*outSPIRV = std::vector(spirv, spirv + numBytes);

	glslang_program_delete(program);
	glslang_shader_delete(shader);

	glslang_finalize_process();
	return VK_SUCCESS;
}

bool endsWith(const char* s, const char* part)
{
	const size_t sLength = strlen(s);
	const size_t partLength = strlen(part);
	return sLength < partLength ? false : strcmp(s + sLength - partLength, part) == 0;
}

VkShaderStageFlagBits vkShaderStageFromFileName(const char* fileName)
{
	if(endsWith(fileName, ".vert"))
		return VK_SHADER_STAGE_VERTEX_BIT;

	if(endsWith(fileName, ".frag"))
		return VK_SHADER_STAGE_FRAGMENT_BIT;

	if(endsWith(fileName, ".geom"))
		return VK_SHADER_STAGE_GEOMETRY_BIT;

	if(endsWith(fileName, ".comp"))
		return VK_SHADER_STAGE_COMPUTE_BIT;

	if(endsWith(fileName, ".tesc"))
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

	if(endsWith(fileName, ".tese"))
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

	return VK_SHADER_STAGE_VERTEX_BIT;
}



IFNITY_API void testShaderCompilation(const char* sourceFilename, const char* destFilename)
{
	std::string shaderSource = Utils::readShaderFile(sourceFilename);

	assert(!shaderSource.empty());

	std::vector<uint8_t> spirv;
	compileShaderVK(vkShaderStageFromFileName(sourceFilename), shaderSource.c_str(), &spirv, glslang_default_resource());

	assert(!spirv.empty());

	saveSPIRVBinaryFile(destFilename, spirv.data(), spirv.size());
}


glslang_resource_t getGlslangResource(const VkPhysicalDeviceLimits& limits)
{
	const glslang_resource_t resource = {
		.max_lights = 32,
		.max_clip_planes = 6,
		.max_texture_units = 32,
		.max_texture_coords = 32,
		.max_vertex_attribs = (int)limits.maxVertexInputAttributes,
		.max_vertex_uniform_components = 4096,
		.max_varying_floats = 64,
		.max_vertex_texture_image_units = 32,
		.max_combined_texture_image_units = 80,
		.max_texture_image_units = 32,
		.max_fragment_uniform_components = 4096,
		.max_draw_buffers = 32,
		.max_vertex_uniform_vectors = 128,
		.max_varying_vectors = 8,
		.max_fragment_uniform_vectors = 16,
		.max_vertex_output_vectors = 16,
		.max_fragment_input_vectors = 15,
		.min_program_texel_offset = -8,
		.max_program_texel_offset = 7,
		.max_clip_distances = (int)limits.maxClipDistances,
		.max_compute_work_group_count_x = (int)limits.maxComputeWorkGroupCount[ 0 ],
		.max_compute_work_group_count_y = (int)limits.maxComputeWorkGroupCount[ 1 ],
		.max_compute_work_group_count_z = (int)limits.maxComputeWorkGroupCount[ 2 ],
		.max_compute_work_group_size_x = (int)limits.maxComputeWorkGroupSize[ 0 ],
		.max_compute_work_group_size_y = (int)limits.maxComputeWorkGroupSize[ 1 ],
		.max_compute_work_group_size_z = (int)limits.maxComputeWorkGroupSize[ 2 ],
		.max_compute_uniform_components = 1024,
		.max_compute_texture_image_units = 16,
		.max_compute_image_uniforms = 8,
		.max_compute_atomic_counters = 8,
		.max_compute_atomic_counter_buffers = 1,
		.max_varying_components = 60,
		.max_vertex_output_components = (int)limits.maxVertexOutputComponents,
		.max_geometry_input_components = (int)limits.maxGeometryInputComponents,
		.max_geometry_output_components = (int)limits.maxGeometryOutputComponents,
		.max_fragment_input_components = (int)limits.maxFragmentInputComponents,
		.max_image_units = 8,
		.max_combined_image_units_and_fragment_outputs = 8,
		.max_combined_shader_output_resources = 8,
		.max_image_samples = 0,
		.max_vertex_image_uniforms = 0,
		.max_tess_control_image_uniforms = 0,
		.max_tess_evaluation_image_uniforms = 0,
		.max_geometry_image_uniforms = 0,
		.max_fragment_image_uniforms = 8,
		.max_combined_image_uniforms = 8,
		.max_geometry_texture_image_units = 16,
		.max_geometry_output_vertices = (int)limits.maxGeometryOutputVertices,
		.max_geometry_total_output_components = (int)limits.maxGeometryTotalOutputComponents,
		.max_geometry_uniform_components = 1024,
		.max_geometry_varying_components = 64,
		.max_tess_control_input_components = (int)limits.maxTessellationControlPerVertexInputComponents,
		.max_tess_control_output_components = (int)limits.maxTessellationControlPerVertexOutputComponents,
		.max_tess_control_texture_image_units = 16,
		.max_tess_control_uniform_components = 1024,
		.max_tess_control_total_output_components = 4096,
		.max_tess_evaluation_input_components = (int)limits.maxTessellationEvaluationInputComponents,
		.max_tess_evaluation_output_components = (int)limits.maxTessellationEvaluationOutputComponents,
		.max_tess_evaluation_texture_image_units = 16,
		.max_tess_evaluation_uniform_components = 1024,
		.max_tess_patch_components = 120,
		.max_patch_vertices = 32,
		.max_tess_gen_level = 64,
		.max_viewports = (int)limits.maxViewports,
		.max_vertex_atomic_counters = 0,
		.max_tess_control_atomic_counters = 0,
		.max_tess_evaluation_atomic_counters = 0,
		.max_geometry_atomic_counters = 0,
		.max_fragment_atomic_counters = 8,
		.max_combined_atomic_counters = 8,
		.max_atomic_counter_bindings = 1,
		.max_vertex_atomic_counter_buffers = 0,
		.max_tess_control_atomic_counter_buffers = 0,
		.max_tess_evaluation_atomic_counter_buffers = 0,
		.max_geometry_atomic_counter_buffers = 0,
		.max_fragment_atomic_counter_buffers = 1,
		.max_combined_atomic_counter_buffers = 1,
		.max_atomic_counter_buffer_size = 16384,
		.max_transform_feedback_buffers = 4,
		.max_transform_feedback_interleaved_components = 64,
		.max_cull_distances = (int)limits.maxCullDistances,
		.max_combined_clip_and_cull_distances = (int)limits.maxCombinedClipAndCullDistances,
		.max_samples = 4,
		.max_mesh_output_vertices_nv = 256,
		.max_mesh_output_primitives_nv = 512,
		.max_mesh_work_group_size_x_nv = 32,
		.max_mesh_work_group_size_y_nv = 1,
		.max_mesh_work_group_size_z_nv = 1,
		.max_task_work_group_size_x_nv = 32,
		.max_task_work_group_size_y_nv = 1,
		.max_task_work_group_size_z_nv = 1,
		.max_mesh_view_count_nv = 4,
		.max_mesh_output_vertices_ext = 256,
		.max_mesh_output_primitives_ext = 512,
		.max_mesh_work_group_size_x_ext = 32,
		.max_mesh_work_group_size_y_ext = 1,
		.max_mesh_work_group_size_z_ext = 1,
		.max_task_work_group_size_x_ext = 32,
		.max_task_work_group_size_y_ext = 1,
		.max_task_work_group_size_z_ext = 1,
		.max_mesh_view_count_ext = 4,
		.maxDualSourceDrawBuffersEXT = 1,
		.limits =
			{
				.non_inductive_for_loops = true,
				.while_loops = true,
				.do_while_loops = true,
				.general_uniform_indexing = true,
				.general_attribute_matrix_vector_indexing = true,
				.general_varying_indexing = true,
				.general_sampler_indexing = true,
				.general_variable_indexing = true,
				.general_constant_matrix_vector_indexing = true,
			},
	};

	return resource;
}




IFNITY_END_NAMESPACE