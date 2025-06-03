#pragma once

#include <pch.h>
#include <functional>
#include <iostream>
#include <Ifnity\Utils\SlotMap.hpp>
IFNITY_NAMESPACE



// -------------------------------------------------------------------
//  Handle<T> forward declaration
// -------------------------------------------------------------------
template <typename T>
struct Handle;


// -------------------------------------------------------------------
//  Holder<T>: RAII wrapper over Handle<T> with custom deleter
// -------------------------------------------------------------------
template <typename ImplObjectType>
using Holder = std::unique_ptr<Handle<ImplObjectType>, std::function<void(Handle<ImplObjectType>*)>>;


// -------------------------------------------------------------------
//  destroy<Device*, Handle<T>>: must be specialized for each backend
// -------------------------------------------------------------------
template <typename HandleT, typename DeviceT>
void destroy(DeviceT* ctx, HandleT handle)
{
    static_assert(sizeof(HandleT) == 0, "destroy<DeviceT*, HandleT> must be specialized for this type.");
}


// -------------------------------------------------------------------
//  Concept: checks if destroy(ctx, handle) is valid
// -------------------------------------------------------------------
template<typename DeviceT, typename HandleT>
concept HasDestroy = requires(DeviceT* ctx, HandleT handle)
{
	{ destroy(ctx, handle) } -> std::same_as<void>;
};


// -------------------------------------------------------------------
//  makeHolder: only available if HasDestroy is satisfied
// -------------------------------------------------------------------
template<typename ImplObjectType, typename DeviceT>
	requires HasDestroy<DeviceT, Handle<ImplObjectType>>
Holder<ImplObjectType> makeHolder(DeviceT* ctx, Handle<ImplObjectType> handle)
{
	return Holder<ImplObjectType>(
		std::make_unique<Handle<ImplObjectType>>(handle).release(),
		[ctx, handle](Handle<ImplObjectType>* ptr)
		{
			if (ctx)
			{
				destroy(ctx, handle);
				std::cout << "[Holder] Destroyed handle at scope exit.\n";
			}
			delete ptr;
		}
	);
}

IFNITY_END_NAMESPACE