#pragma once

struct VertexData
{
    int16_t chunk_x;
    int16_t chunk_y;
    uint8_t x : 4;
    uint8_t z : 4;
    uint8_t y;
    uint8_t texCoords : 2;
};