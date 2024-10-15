// Created: 23.02.2018 16:00

#pragma once

#include "pch.h"

IFNITY_NAMESPACE


enum class FolderType
{
	Shaders,
	Mesh,
	Scenes,
	Cameras
};

class IFNITY_API VFS
{
public:
    static VFS& GetInstance();

    bool Mount(const std::string& virtualPath, const std::string& physicalPath, FolderType type);
    bool Unmount(const std::string& virtualPath);
    std::string ResolvePath(const std::string& virtualPath) const;
    bool SaveFile(const std::string& virtualPath, const std::string& fileName, const std::vector<char>& data) const;
    std::vector<std::string> VFS::ListFiles(const std::string& virtualPath, const std::string& subdirectory) const;
private:
    VFS() = default;
    ~VFS() = default;

    VFS(const VFS&) = delete;
    VFS& operator=(const VFS&) = delete;

    std::unordered_map<std::string, std::vector<std::string>> m_mountPoints;
};


IFNITY_END_NAMESPACE

