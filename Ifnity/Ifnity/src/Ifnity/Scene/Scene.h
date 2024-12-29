//This library is a based a Chapter 7 of the book 3D Graphics Rendering Cookbook by Sergey Kosarevsky and Viktor Latypov



#pragma once
#include "pch.h"

#include <glm/glm.hpp>
#include <glm/ext.hpp>


IFNITY_NAMESPACE

using glm::mat4;



struct IFNITY_API SceneConfig
{
	std::string fileName;
	std::string outputMesh;
	std::string outputScene;
	std::string outputMaterials;
	std::string outputBoxes;
	float scale;
	bool calculateLODs;
	bool mergeInstances;
};

// we do not define std::vector<Node*> Children - this is already present in the aiNode from assimp

constexpr const int MAX_NODE_LEVEL = 16;

struct Hierarchy
{
	// parent for this node (or -1 for root)
	int parent_;
	// first child for a node (or -1)
	int firstChild_;
	// next sibling for a node (or -1)
	int nextSibling_;
	// last added node (or -1)
	int lastSibling_;
	// cached node level
	int level_;

	// comparison operator for testing purposes
	bool operator==(const Hierarchy& rhs) const
	{
		return parent_ == rhs.parent_ &&
			firstChild_ == rhs.firstChild_ &&
			nextSibling_ == rhs.nextSibling_ &&
			lastSibling_ == rhs.lastSibling_ &&
			level_ == rhs.level_;
	}
};

/* This scene is converted into a descriptorSet(s) in MultiRenderer class
   This structure is also used as a storage type in SceneExporter tool
 */
struct  IFNITY_API Scene
{
	// local transformations for each node and global transforms
	// + an array of 'dirty/changed' local transforms
	std::vector<mat4> localTransform_;
	std::vector<mat4> globalTransform_;

	// list of nodes whose global transform must be recalculated
	std::vector<int> changedAtThisFrame_[ MAX_NODE_LEVEL ];

	// Hierarchy component
	std::vector<Hierarchy> hierarchy_;

	// Mesh component: Which node corresponds to which node
	std::unordered_map<uint32_t, uint32_t> meshes_;

	// Material component: Which material belongs to which node
	std::unordered_map<uint32_t, uint32_t> materialForNode_;

	// Node name component: Which name is assigned to the node
	std::unordered_map<uint32_t, uint32_t> nameForNode_;

	// List of scene node names
	std::vector<std::string> names_;

	// Debug list of material names
	std::vector<std::string> materialNames_;

	bool operator==(const Scene& rhs) const
	{
		return localTransform_ == rhs.localTransform_ &&
			globalTransform_ == rhs.globalTransform_ &&
			hierarchy_ == rhs.hierarchy_ &&
			meshes_ == rhs.meshes_ &&
			materialForNode_ == rhs.materialForNode_ &&
			nameForNode_ == rhs.nameForNode_ &&
			names_ == rhs.names_ &&
			materialNames_ == rhs.materialNames_;
	};

};



/**
 * @brief Adds a new node to the scene.
 * 
 * @param scene The scene to which the node will be added.
 * @param parent The parent node index. Use -1 for root.
 * @param level The level of the node in the hierarchy.
 * @return The index of the newly added node.
 */
int addNode(Scene& scene, int parent, int level);

/**
 * @brief Marks a node as changed in the scene.
 *
 * @param scene The scene in which the node is located.
 * @param node The index of the node to mark as changed.
 */
void markAsChanged(Scene& scene, int node);

/**
 * @brief Finds a node by name in the scene.
 *
 * @param scene The scene in which to search for the node.
 * @param name The name of the node to find.
 * @return The index of the node with the specified name, or -1 if no such node exists.
 */
int findNodeByName(const Scene& scene, const std::string& name);

/**
 * @brief Gets the name of a node in the scene.
 *
 * @param scene The scene in which the node is located.
 * @param node The index of the node.
 * @return The name of the node.
 */
inline std::string getNodeName(const Scene& scene, int node)
{

	auto it = scene.nameForNode_.find(node);
	int strID = (it != scene.nameForNode_.end()) ? it->second : -1;
	return (strID > -1) ? scene.names_[ strID ] : std::string();
}


/**
 * @brief Sets the name of a node in the scene.
 *
 * @param scene The scene in which the node is located.
 * @param node The index of the node.
 * @param name The name to set for the node.
 */
inline void setNodeName(Scene& scene, int node, const std::string& name)
{
	uint32_t stringID = (uint32_t)scene.names_.size();
	scene.names_.push_back(name);
	scene.nameForNode_[ node ] = stringID;
}

/**
 * @brief Gets the level of a node in the scene.
 *
 * @param scene The scene in which the node is located.
 * @param n The index of the node.
 * @return The level of the node.
 */
int getNodeLevel(const Scene& scene, int n);

/**
 * @brief Recalculates the global transforms for all nodes in the scene.
 *
 * @param scene The scene for which to recalculate the global transforms.
 */
void recalculateGlobalTransforms(Scene& scene);

/**
 * @brief Loads a scene from a file.
 *
 * @param fileName The name of the file from which to load the scene.
 * @param scene The scene to which the loaded scene will be added.
 */
void loadScene(const char* fileName, Scene& scene);

/**
 * @brief Saves a scene to a file.
 *
 * @param fileName The name of the file to which to save the scene.
 * @param scene The scene to save.
 */
void saveScene(const char* fileName, const Scene& scene);

/**
 * @brief Dumps the transforms of the scene to a file.
 *
 * @param fileName The name of the file to which to dump the transforms.
 * @param scene The scene whose transforms to dump.
 */
void dumpTransforms(const char* fileName, const Scene& scene);


/**
 * @brief Prints the nodes that have changed in the scene.
 *
 * @param scene The scene in which to print the changed nodes.
 */
void printChangedNodes(const Scene& scene);

/**
 * @brief Dumps the scene to a DOT file.
 *
 * @param fileName The name of the file to which to dump the scene.
 * @param scene The scene to dump.
 * @param visited The visited nodes.
 */
void dumpSceneToDot(const char* fileName, const Scene& scene, int* visited = nullptr);

void mergeScenes(Scene& scene, const std::vector<Scene*>& scenes, const std::vector<glm::mat4>& rootTransforms, const std::vector<uint32_t>& meshCounts,
	bool mergeMeshes = true, bool mergeMaterials = true);

/**
 * @brief Shifts the nodes in the scene.
 *
 * @param scene The scene in which to shift the nodes..
 * @param nodesToDelete The nodes to delete.
 */
void deleteSceneNodes(Scene& scene, const std::vector<uint32_t>& nodesToDelete);



/**
 * @brief Reads the scene configuration from a file.
 *
 * @param fileName The name of the file from which to read the scene configuration.
 * @return A vector of SceneConfig objects read from the file.
 */
std::vector<SceneConfig> readSceneConfig(const char* fileName);

void traverse(const aiScene* sourceScene, Scene& scene, aiNode* N, int parent, int ofs);





IFNITY_END_NAMESPACE