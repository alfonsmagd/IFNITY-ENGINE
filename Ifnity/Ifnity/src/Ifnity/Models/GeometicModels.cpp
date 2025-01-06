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

    Tetrahedron::~Tetrahedron()
    {
        vertices.clear();
        index.clear();
    }



    //Cube implementation 

    Cube::Cube()
    {
        vertices =
        {
            // Vértices del cubo con sus posiciones, normales y tangentes
            {{-1.0f, -1.0f, -1.0f}, {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}},
            {{1.0f, -1.0f, -1.0f}, {1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
            {{1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, -1.0f}, {1.0f, 1.0f, 0.0f}},
            {{-1.0f, 1.0f, -1.0f}, {-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}},
            {{1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 1.0f}},
            {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}},
            {{-1.0f, 1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}},
        };

        index =
        {
            0, 1, 2, 2, 3, 0, // Cara 1
            4, 5, 6, 6, 7, 4, // Cara 2
            0, 1, 5, 5, 4, 0, // Cara 3
            2, 3, 7, 7, 6, 2, // Cara 4
            0, 3, 7, 7, 4, 0, // Cara 5
            1, 2, 6, 6, 5, 1  // Cara 6
        };
    }

    Cube::~Cube()
    {
        vertices.clear();
        index.clear();
    }

    


}

IFNITY_END_NAMESPACE
