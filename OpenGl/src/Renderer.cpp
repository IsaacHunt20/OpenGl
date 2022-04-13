#include "Render.h"
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