#include "Renderer.h"
#include<iostream>

void GLCLearError()
{
    //loop to run until all errors are cleared
    while (glGetError() != GL_NO_ERROR);
    //runs until number of errors is zero
}

bool GLLogCall(const char* function, const char* file, int line)
{
    //While there is errors write them out 
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << "): " << function << " " << file << ": " << line << std::endl;
        return false;
    }
    return true;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const
{
    shader.Bind();
    va.Bind();
    ib.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount() , GL_UNSIGNED_INT, nullptr));
}
