

#pragma once

#include "../../Graphics/ifrhi.h"

IFNITY_NAMESPACE

class ICommandListHandle

{


	/**
	 * @brief Opens the command list.
	 */
	virtual void Open() = 0;

	/**
	 * @brief Closes the command list.
	 */
	virtual void Close() = 0;

	/**
	 * @brief Draws the specified description.
	 *
	 * @param desc The draw description.
	 */
	virtual void Draw(rhi::DrawDescription& desc) = 0;

};


IFNITY_END_NAMESPACE