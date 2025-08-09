

#pragma once 


#include "pch.h"

IFNITY_NAMESPACE

struct IRenderPassVisitor {
	virtual void Visit(class SimpleRenderer& pass) = 0;
	virtual void Visit( class DebugRenderer& pass ) = 0;
	//virtual void Visit(class ShadowRenderer& pass) = 0;
	virtual ~IRenderPassVisitor() = default;
};

IFNITY_END_NAMESPACE