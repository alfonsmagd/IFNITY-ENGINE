#pragma once

#include <pch.h>

IFNITY_NAMESPACE



template<typename ImplObjType>
class Handle final
{
public:
	Handle() = default;

	bool valid() const { return gen_ != 0; }
	uint32_t index() const { return index_; }
	uint32_t gen() const { return gen_; }
	bool empty() const
	{
		return gen_ == 0;
	}

	bool operator==(const Handle& other) const
	{
		return index_ == other.index_ && gen_ == other.gen_;
	}
	bool operator!=(const Handle& other) const
	{
		return !(*this == other);
	}
	// allow conditions 'if (handle)'
	explicit operator bool() const
	{
		return gen_ != 0;
	}
	// Operator uint32_t
	operator uint32_t() const
	{
		if( gen_ != 0 )
		{
			return index_;
		}
		return 0xFFFFF; // Devuelve un valor predeterminado cuando gen_ es 0
	}

private:
	Handle(uint32_t index, uint32_t gen): index_(index), gen_(gen) {}

	template<typename T>
	friend class SlotMap;

	uint32_t index_ = 0;
	uint32_t gen_ = 0;
};

// SlotMap sin nextFree, usando un vector como free list
template<typename ObjectType>
class SlotMap final
{
	struct Slot
	{
		ObjectType obj = {};
		uint32_t gen = 1;
	};

	
	std::vector<uint32_t> freeList; // Pila de �ndices libres

public:
	std::vector<Slot> slots;

	Handle<ObjectType> create(ObjectType&& obj)
	{
		uint32_t index;
		if( !freeList.empty() )
		{
			index = freeList.back(); // Reutiliza �ndice
			freeList.pop_back();
			slots[ index ].obj = std::move(obj);
		}
		else
		{
			index = static_cast<uint32_t>(slots.size());
			slots.push_back({ std::move(obj), 1 });
		}
		return Handle<ObjectType>(index, slots[ index ].gen);
	}

	void destroy(Handle<ObjectType> handle)
	{
		if( !handle.valid() ) return;
		uint32_t index = handle.index();
		assert(index < slots.size() && handle.gen() == slots[ index ].gen);

		slots[ index ].gen++; // Invalida el handle viejo
		freeList.push_back(index); // A�ade el �ndice a la free list
	}

	ObjectType* get(Handle<ObjectType> handle)
	{
		if( !handle.valid() ) return nullptr;
		uint32_t index = handle.index();
		assert(index < slots.size() && handle.gen() == slots[ index ].gen);
		return &slots[ index ].obj;
	}

	const ObjectType* get(Handle<ObjectType> handle) const
	{
		if( !handle.valid() ) return nullptr;
		uint32_t index = handle.index();
		assert(index < slots.size() && handle.gen() == slots[ index ].gen);
		return &slots[ index ].obj;
	}
	ObjectType* getByIndex(uint32_t index)
	{
		if( index >= slots.size() ) return nullptr;  // Verifica si el �ndice est� fuera de rango
		const auto& slot = slots[ index ];
		if( slot.gen == 0 ) return nullptr;  // Verifica que la generaci�n no sea 0, lo que indica un objeto inv�lido
		return &slot.obj;
	}

	const ObjectType* getByIndex(uint32_t index) const
	{
		if( index >= slots.size() ) return nullptr;  // Verifica si el �ndice est� fuera de rango
		const auto& slot = slots[ index ];
		if( slot.gen == 0 ) return nullptr;  // Verifica que la generaci�n no sea 0
		return &slot.obj;
	}


	//Find Object by ObjectType
	Handle<ObjectType> find(const ObjectType* obj)
	{
		if( !obj ) return {};
		for( uint32_t i = 0; i < slots.size(); i++ )
		{
			if( &slots[ i ].obj == obj )
			{
				return Handle<ObjectType>(i, slots[ i ].gen);
			}
		}

	}

	uint32_t numObjects() const { return static_cast<uint32_t>(slots.size() - freeList.size()); }

	void clear()
	{
		slots.clear();
		freeList.clear();
	}

	uint32_t size() const { return static_cast<uint32_t>(slots.size() - freeList.size()); }
};

static_assert(sizeof(Handle<class Foo>) == sizeof(uint64_t));


IFNITY_END_NAMESPACE