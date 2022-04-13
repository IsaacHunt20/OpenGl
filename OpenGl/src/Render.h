#pragma once
#include<GL/glew.h>
//Error Checking for all GL functions this is wrapped by should throough breakpoint but it isn't TODO thing
//For now will just return function file line 
#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x) GLCLearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__,__LINE__))


void GLCLearError();
bool GLLogCall(const char* function, const char* file, int line);
