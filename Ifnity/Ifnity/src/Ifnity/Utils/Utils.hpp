#pragma once

#if !defined(_CRT_SECURE_NO_WARNINGS)
#	define _CRT_SECURE_NO_WARNINGS 1
#endif // _CRT_SECURE_NO_WARNINGS

#include <malloc.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <vector>



template <typename T>
inline void mergeVectors(std::vector<T>& v1, const std::vector<T>& v2)
{
	v1.insert(v1.end(), v2.begin(), v2.end());
}

inline int addUnique(std::vector<std::string>& files, const std::string& file)
{
	if(file.empty())
		return -1;

	auto i = std::find(std::begin(files), std::end(files), file);

	if(i == files.end())
	{
		files.push_back(file);
		return (int)files.size() - 1;
	}

	return (int)std::distance(files.begin(), i);
}

template <class T, class Index = int>
inline void eraseSelected(std::vector<T>& v, const std::vector<Index>& selection)
{
    if(selection.empty()) return;

    // Crear un vector temporal para almacenar los elementos que no se eliminar�n
    std::vector<T> temp;
    temp.reserve(v.size() - selection.size());

    // �ndice de la selecci�n
    size_t sel_idx = 0;
    size_t sel_size = selection.size();

    // Recorrer el vector original y copiar los elementos que no est�n en la selecci�n
    for(size_t i = 0; i < v.size(); ++i)
    {
        if(sel_idx < sel_size && static_cast<Index>(i) == selection[ sel_idx ])
        {
            ++sel_idx; // Saltar los elementos que est�n en la selecci�n
        }
        else
        {
            temp.push_back(std::move(v[ i ])); // Mover los elementos no seleccionados al vector temporal
        }
    }

    // Intercambiar el contenido del vector original con el vector temporal
    v.swap(temp);
}


template<typename T>
concept StringOrCharPointer = std::is_same_v<T, std::string> || std::is_same_v<T, const char*>;

template <StringOrCharPointer T>
bool compareVectors(const std::vector<T>& vec1, const std::vector<T>& vec2)
{
    if(vec1.size() != vec2.size())
        return false;

    for(size_t i = 0; i < vec1.size(); ++i)
    {
        if(vec1[ i ] != vec2[ i ])
            return false;
    }

    return true;
}



//macros define to destroy objects in deferred way.
#define DESTROY_VK_STAGING_BUFFER_DEFFERED(device, stagingBuffer, stagingMemory) \
    std::packaged_task<void()>([device, stagingBuffer, stagingMemory]() \
    { \
        vkDestroyBuffer(device, stagingBuffer, nullptr); \
        vkFreeMemory(device, stagingMemory, nullptr); \
    })

#define DESTROY_VK_PIPELINE_DEFERRED(device, pipeline) \
    std::packaged_task<void()>([device, pipeline]() \
    { \
        vkDestroyPipeline(device, pipeline, nullptr); \
    })

#define DESTROY_VK_PIPELINE_LAYOUT_DEFERRED(device, pipelineLayout) \
    std::packaged_task<void()>([device, pipelineLayout]() \
    { \
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr); \
    })

#define DESTROY_VK_DESCRIPTOR_SET_LAYOUT_DEFERRED(device, descriptorSetLayout) \
    std::packaged_task<void()>([device, descriptorSetLayout]() \
    { \
        vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr); \
    })
#define DESTROY_VK_DESCRIPTOR_POOL_DEFERRED(device, descriptorPool) \
    std::packaged_task<void()>([device, descriptorPool]() \
    { \
        vkDestroyDescriptorPool(device, descriptorPool, nullptr); \
    })






#define DESTROY_D3D12_BUFFER_DEFERRED_MA(resource, allocation) \
    std::packaged_task<void()>([resource, allocation]() mutable { \
        if (resource) resource.Reset(); \
        if (allocation) allocation->Release(); \
    })


#define DESTROY_D3D12_BUFFER_DEFERRED(resource) \
    std::packaged_task<void()>([resource]() mutable { \
        if (resource) resource.Reset(); \
    })