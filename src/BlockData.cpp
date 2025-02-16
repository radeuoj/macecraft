#include "BlockData.h"

namespace Macecraft
{
BlockData::BlockData(uint8_t texture)
{
    textureZP = textureZN = textureXP = textureXN = textureYP = textureYN = texture;
}

BlockData::BlockData(uint8_t _textureZP, uint8_t _textureZN, uint8_t _textureXP, uint8_t _textureXN, uint8_t _textureYP, uint8_t _textureYN):
textureZP(_textureZP), textureZN(_textureZN), textureXP(_textureXP), textureXN(_textureXN), textureYP(_textureYP), textureYN(_textureYN)
{
}

uint8_t& BlockData::operator[](BlockDirection direction)
{
    switch (direction)
    {
    case BlockDirection::ZP: return textureZP;
    case BlockDirection::ZN: return textureZN;
    case BlockDirection::XP: return textureXP;
    case BlockDirection::XN: return textureXN;
    case BlockDirection::YP: return textureYP;
    case BlockDirection::YN: return textureYN;
    default: return textureZP;
    }
}
}


