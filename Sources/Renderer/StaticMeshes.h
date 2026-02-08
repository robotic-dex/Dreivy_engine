#pragma once
#include "MeshData.h"

inline MeshData CreateTestCube()
{
    MeshData mesh;

    // 1 x 3 x 1 box (X, Y, Z)
    mesh.positions = {
        { -0.5f, -1.5f, -0.5f },
        { -0.5f,  1.5f, -0.5f },
        {  0.5f,  1.5f, -0.5f },
        {  0.5f, -1.5f, -0.5f },

        { -0.5f, -1.5f,  0.5f },
        { -0.5f,  1.5f,  0.5f },
        {  0.5f,  1.5f,  0.5f },
        {  0.5f, -1.5f,  0.5f }
    };

    mesh.indices = {
        0,1,2, 0,2,3,
        4,6,5, 4,7,6,
        4,5,1, 4,1,0,
        3,2,6, 3,6,7,
        1,5,6, 1,6,2,
        4,0,3, 4,3,7
    };

    return mesh;
}
