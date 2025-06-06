// Created: 23.02.2018 16:00

#pragma once

#include "pch.h"

IFNITY_NAMESPACE


/**
 * @enum FolderType
 * @brief Enum representing different types of folders.
 */
    enum class FolderType
{
    SHADERS,
    MESH,
    SCENES,
    CAMERAS,
    TEXTURES,

    NO_DEFINED
};

/**
 * @class VFS
 * @brief Virtual File System class for managing virtual and physical paths.
 */
class IFNITY_API VFS
{
public:
    /**
     * @brief Gets the singleton instance of the VFS.
     * @param physicalPath The physical path to initialize the VFS with, if it hasn't been created yet.
     * @return Reference to the VFS instance.
     */
    static VFS& GetInstance(const std::string& physicalPath = "data");

	

    /**
     * @brief Mounts a physical path to a virtual path.
     * @param virtualPath The virtual path.
     * @param physicalPath The physical path.
     * @param type The type of folder, used for creating subdirectories. See @ref FolderType.
     * @return true if the mount was successful, false otherwise.
     */
    bool Mount(const std::string& virtualPath, const std::string& physicalPath, FolderType type);

    /**
     * @brief Unmounts a virtual path.
     * @param virtualPath The virtual path to unmount.
     * @return true if the unmount was successful, false otherwise.
     */
    bool Unmount(const std::string& virtualPath);

    /**
     * @brief Resolves a virtual path to a physical path.
     * @param virtualPath The virtual path.
     * @return The resolved physical path.
     */
    std::string ResolvePath(const std::string& virtualPath) const;

    /**
     * @brief Resolves a virtual path and subdirectory to a physical path.
     * @param virtualPath The virtual path.
     * @param subdirectory The subdirectory.
     * @return The resolved physical path.
     */
    std::string ResolvePath(const std::string& virtualPath, const std::string& subdirectory) const;

    /**
     * @brief Saves a file to the virtual file system.
     * @param virtualPath The virtual path.
     * @param fileName The name of the file.
     * @param data The data to save.
     * @return The path where the file was saved.
     */
    std::string SaveFile(const std::string& virtualPath, const std::string& fileName, const std::vector<char>& data) const;

    /**
     * @brief Lists files in a virtual path and subdirectory.
     * @param virtualPath The virtual path.
     * @param subdirectory The subdirectory.
     * @return A vector of file names.
     */
    std::vector<std::string> ListFiles(const std::string& virtualPath, const std::string& subdirectory) const;


    std::vector<std::string> ListFilesInCurrentDirectory(const std::string& virtualPath, bool onlyfilename = false) const;


    void RegisterFile(const std::string& virtualPath, const std::string& filePath);
  

private:
    VFS() = default;
    VFS(const std::string& physicalPath);
    ~VFS() = default;

    VFS(const VFS&) = delete;
    VFS& operator=(const VFS&) = delete;

    /// Map of mount points.
    std::unordered_map<std::string, std::vector<std::string>> m_MountPoints;
};

IFNITY_END_NAMESPACE



