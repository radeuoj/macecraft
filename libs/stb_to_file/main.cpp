#include <iostream>
#include <fstream>
#include <stb_image/stb_image.h>

int main()
{
    int x = 0, y = 0, channels = 0;
    unsigned char* bytes = stbi_load("dirt.png", &x, &y, &channels, 0);

    std::ofstream fout("dirt.png.bytes");
    fout << bytes;
}