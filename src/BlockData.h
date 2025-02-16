#pragma once

#include <cstdint>

namespace Macecraft
{
enum class BlockDirection: uint8_t
{
    ZP = 0,
    ZN,
    XP,
    XN,
    YP,
    YN,
};

struct BlockData
{
    BlockData() = default;
    BlockData(uint8_t texture);
    BlockData(uint8_t _textureZP, uint8_t _textureZN, uint8_t _textureXP, uint8_t _textureXN, uint8_t _textureYP, uint8_t _textureYN);
    uint8_t textureZP, textureZN, textureXP, textureXN, textureYP, textureYN;
    uint8_t& operator[](BlockDirection direction);
};
}
