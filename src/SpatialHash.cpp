// #include "SpatialHash.hpp"



// SpatialHash::SpatialHash(int size)
//     : size(size)
// {
//     gridIds.resize(size, std::vector<std::vector<int>>(0));
//     gridCounts.resize(size, std::vector<int>(0, 0));
// }

// SpatialHash::~SpatialHash()
// {
// }

// void SpatialHash::mapGrid(std::vector<Vector2>& positions)
// {
//     // reset counts
//     for (int r = 0; r < size; r++)
//         for (unsigned int c = 0; c < size; c++)
//             gridCounts[r][c] = 0;
    
//     // remap ids and recaculate counts
//     for (int i = 0; i < positions.size(); i++) {
//         const int r = positions[i].y / 2.0f;
//         const int c = positions[i].x / 2.0f;

//         // if capacity reached, resize gridId vector
//         if (gridCounts[r][c] >= (int)(gridIds[r][c].capacity()))
//              gridIds[r][c].resize((int)(1.5 * gridIds[r][c].capacity()));

//         // add map id and increment counter
//         gridIds[r][c][gridCounts[r][c]++] = i;
//     }
// }

