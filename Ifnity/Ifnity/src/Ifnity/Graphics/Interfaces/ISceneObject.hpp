#pragma once 


#include "pch.h"



IFNITY_NAMESPACE

//Forward delcaration 


class IFNITY_API ISceneObject
{
public:
    virtual ~ISceneObject() = default;

    virtual const MeshFileHeader& getHeader() const = 0;
    virtual const MeshData& getMeshData() const = 0;
    virtual const Scene& getScene() const = 0;
    virtual const std::vector<MaterialDescription>& getMaterials() const = 0;
    virtual const std::vector<DrawData>& getShapes() const = 0;
};

using SceneObjectHandler = std::shared_ptr<ISceneObject>;



IFNITY_END_NAMESPACE