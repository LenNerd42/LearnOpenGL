#pragma once

unsigned int loadTexture(char const* path);

struct VertexAttribPointer
{
    int size;
    int type;
};

class VertexAttribBuilder
{
public:
    VertexAttribBuilder()
    {
    }

    ~VertexAttribBuilder()
    {
    }

    // Add an attribute at the end of the vertex attrib array.
    void AddAttribute(int size, int type);
    // Set all vertex attributes from the list of previously supplied attributes.
    void Finalize() const;

private:
    static constexpr int maxVertexAttribs = 8;
    VertexAttribPointer pointers[maxVertexAttribs];
    int nextIndex = 0;
    int stride = 0;
};