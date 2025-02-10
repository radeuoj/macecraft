#include "BlockData.h"

Macecraft::BlockData::BlockData(uint8_t texture)
{
    textureZP = textureZN = textureXP = textureXN = textureYP = textureYN = texture;
}

Macecraft::BlockData::BlockData(uint8_t _textureZP, uint8_t _textureZN, uint8_t _textureXP, uint8_t _textureXN, uint8_t _textureYP, uint8_t _textureYN):
textureZP(_textureZP), textureZN(_textureZN), textureXP(_textureXP), textureXN(_textureXN), textureYP(_textureYP), textureYN(_textureYN)
{
}
