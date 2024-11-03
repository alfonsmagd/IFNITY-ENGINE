#include "GeometicModels.hpp"

IFNITY_NAMESPACE

namespace GeometricModels
{
    Tetrahedron::Tetrahedron()
    {
        vertices =
        {
            // Vértices del tetraedro con sus posiciones, normales y tangentes
            {{1.0f, 1.0f, 1.0f},   {0.577f, 0.577f, 0.577f},   {1.0f, 0.0f, 0.0f}},
            {{-1.0f, -1.0f, 1.0f}, {-0.577f, -0.577f, 0.577f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, 1.0f, -1.0f}, {-0.577f, 0.577f, -0.577f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, -1.0f, -1.0f}, {0.577f, -0.577f, -0.577f}, {1.0f, 1.0f, 0.0f}},
        };

        index =
        {
            0, 1, 2, // Cara 1
            0, 3, 1, // Cara 2
            0, 2, 3, // Cara 3
            1, 3, 2  // Cara 4
        };
    }
}

IFNITY_END_NAMESPACE
