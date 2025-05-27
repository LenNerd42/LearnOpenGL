#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <glad/glad.h>
#include "stb_image.h"
#include "Util.h"

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    // Load image data for texture.
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        // Bind data to current texture object and generate mipmaps (lower resolution textures).
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Texture wrapping will repeat texture and texture filtering uses mipmaps and linear filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Texture bound, now free image memory.
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void VertexAttribBuilder::AddAttribute(const int size, const int type)
{
    if (nextIndex >= maxVertexAttribs) return;
    
    VertexAttribPointer pointer = {size, type};
    pointers[nextIndex] = pointer;
    nextIndex++;
    stride += size;
}

void VertexAttribBuilder::Finalize() const
{
    int offset = 0;
    for (int i = 0; i < nextIndex; i++)
    {
        const VertexAttribPointer pointer = pointers[i];
        
        glVertexAttribPointer(i, pointer.size, pointer.type, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));
        glEnableVertexAttribArray(i);
        
        offset += pointer.size;
    }
}
