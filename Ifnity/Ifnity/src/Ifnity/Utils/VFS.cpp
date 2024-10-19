
// Created: 23.02.2018 16:00

#pragma once
#include "VFS.hpp"

IFNITY_NAMESPACE




VFS& VFS::GetInstance()
{
    static VFS instance;
    return instance;
}

bool VFS::Mount(const std::string& virtualPath, const std::string& physicalPath , FolderType type)
{
    auto& paths = m_MountPoints[virtualPath];

    if (type == FolderType::SHADERS)
    {
        std::filesystem::create_directories(physicalPath + "/opengl");
        std::filesystem::create_directories(physicalPath + "/d3d12");
        std::filesystem::create_directories(physicalPath + "/vk");
    }

    if (std::find(paths.begin(), paths.end(), physicalPath) == paths.end())
    {
        paths.push_back(physicalPath);
        if (type == FolderType::SHADERS)
        {
            paths.push_back(physicalPath + "/opengl");
            paths.push_back(physicalPath + "/d3d12");
            paths.push_back(physicalPath + "/vk");
        }
        return true;
    }
    return false;
}

bool VFS::Unmount(const std::string& virtualPath)
{
    return m_MountPoints.erase(virtualPath) > 0;
}

std::string VFS::ResolvePath(const std::string& virtualPath) const
{
    auto it = m_MountPoints.find(virtualPath);
    if (it != m_MountPoints.end() && !it->second.empty())
    {
        return it->second.front(); // Devuelve el primer path físico montado
    }
    return "";
}

std::string VFS::ResolvePath(const std::string& virtualPath, const std::string& subdirectory) const
{
    auto it = m_MountPoints.find(virtualPath);
    if(it != m_MountPoints.end() && !it->second.empty())
    {
        for(const auto& physicalPath : it->second)
        {
            std::string fullPath = physicalPath + (subdirectory.empty() ? "" : "/" + subdirectory);
            if(std::filesystem::exists(fullPath))
            {
                return fullPath; // Devuelve la primera ruta física que existe
            }
        }
    }
    return "";
}

std::string VFS::SaveFile(const std::string& virtualPath, const std::string& fileName, const std::vector<char>& data) const
{
    std::string physicalPath = ResolvePath(virtualPath);
    if (physicalPath.empty())
    {
        return false;
    }

    // Determinar el subdirectorio basado en la extensión del archivo
    std::string extension = fileName.substr(fileName.find_last_of('.') + 1);
    std::string subdirectory;

    if (extension == "glsl")
    {
        subdirectory = "opengl";
    }
    else if (extension == "hlsl")
    {
        subdirectory = "d3d12";
    }
    else if (extension == "spv" || extension == "spirv")
    {
        subdirectory = "vk";
    }
    else
    {
        subdirectory = "";
    }

    std::string fullPath = physicalPath + (subdirectory.empty() ? "" : "/" + subdirectory) + "/" + fileName;
    std::ofstream outFile(fullPath, std::ios::binary);
    if (!outFile)
    {
        return false;
    }

    outFile.write(data.data(), data.size());
    outFile.close();
    return fullPath;
}


std::vector<std::string> VFS::ListFiles(const std::string& virtualPath, const std::string& subdirectory) const
{
    std::vector<std::string> fileList;
    std::string physicalPath = ResolvePath(virtualPath);
    if (physicalPath.empty())
    {
        return fileList;
    }

    if (!subdirectory.empty())
    {
        physicalPath += "/" + subdirectory;
    }

    for (const auto& entry : std::filesystem::directory_iterator(physicalPath))
    {
        if (entry.is_regular_file())
        {
            fileList.push_back(entry.path().filename().string());
        }
    }

    return fileList;
}

IFNITY_END_NAMESPACE

