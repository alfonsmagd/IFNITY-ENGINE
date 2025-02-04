#pragma once

#include <pch.h>

IFNITY_NAMESPACE



template<typename ObjectType>
class Handle
{
public:
    Handle() = default;

    bool valid() const { return gen_ != 0; }
    uint32_t index() const { return index_; }
    uint32_t gen() const { return gen_; }

    bool operator==(const Handle& other) const
    {
        return index_ == other.index_ && gen_ == other.gen_;
    }
    bool operator!=(const Handle& other) const
    {
        return !(*this == other);
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
class SlotMap
{
    struct Slot
    {
        ObjectType obj;
        uint32_t gen = 1;
    };

    std::vector<Slot> slots;
    std::vector<uint32_t> freeList; // Pila de índices libres

public:
    Handle<ObjectType> create(ObjectType&& obj)
    {
        uint32_t index;
        if(!freeList.empty())
        {
            index = freeList.back(); // Reutiliza índice
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
        if(!handle.valid()) return;
        uint32_t index = handle.index();
        assert(index < slots.size() && handle.gen() == slots[ index ].gen);

        slots[ index ].gen++; // Invalida el handle viejo
        freeList.push_back(index); // Añade el índice a la free list
    }

    ObjectType* get(Handle<ObjectType> handle)
    {
        if(!handle.valid()) return nullptr;
        uint32_t index = handle.index();
        assert(index < slots.size() && handle.gen() == slots[ index ].gen);
        return &slots[ index ].obj;
    }

    const ObjectType* get(Handle<ObjectType> handle) const
    {
        return const_cast<SlotMap*>(this)->get(handle);
    }

    void clear()
    {
        slots.clear();
        freeList.clear();
    }

    uint32_t size() const { return static_cast<uint32_t>(slots.size() - freeList.size()); }
};










IFNITY_END_NAMESPACE