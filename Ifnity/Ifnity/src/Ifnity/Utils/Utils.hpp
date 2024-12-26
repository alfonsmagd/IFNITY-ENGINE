#pragma once

#if !defined(_CRT_SECURE_NO_WARNINGS)
#	define _CRT_SECURE_NO_WARNINGS 1
#endif // _CRT_SECURE_NO_WARNINGS

#include <malloc.h>
#include <string.h>
#include <algorithm>
#include <string>
#include <vector>



template <typename T>
inline void mergeVectors(std::vector<T>& v1, const std::vector<T>& v2)
{
	v1.insert(v1.end(), v2.begin(), v2.end());
}

inline int addUnique(std::vector<std::string>& files, const std::string& file)
{
	if(file.empty())
		return -1;

	auto i = std::find(std::begin(files), std::end(files), file);

	if(i == files.end())
	{
		files.push_back(file);
		return (int)files.size() - 1;
	}

	return (int)std::distance(files.begin(), i);
}

template <class T, class Index = int>
inline void eraseSelected(std::vector<T>& v, const std::vector<Index>& selection)
{
    if(selection.empty()) return;

    // Crear un vector temporal para almacenar los elementos que no se eliminarán
    std::vector<T> temp;
    temp.reserve(v.size() - selection.size());

    // Índice de la selección
    size_t sel_idx = 0;
    size_t sel_size = selection.size();

    // Recorrer el vector original y copiar los elementos que no están en la selección
    for(size_t i = 0; i < v.size(); ++i)
    {
        if(sel_idx < sel_size && static_cast<Index>(i) == selection[ sel_idx ])
        {
            ++sel_idx; // Saltar los elementos que están en la selección
        }
        else
        {
            temp.push_back(std::move(v[ i ])); // Mover los elementos no seleccionados al vector temporal
        }
    }

    // Intercambiar el contenido del vector original con el vector temporal
    v.swap(temp);
}