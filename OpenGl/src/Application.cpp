#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include<fstream>
#include<cstdio>
#include<iostream>
#include<string>
#include<sstream>

//Macro to define our assertion
#define ASSERT(x) if ((!x)) __debugbreak();
#define GLCall(x) GLCLearError();\
    x;\
    ASSERT(GLLogCall())


static void GLCLearError()
{
    //loop to run until all errors are cleared
    while (glGetError() != GL_NO_ERROR);
    //runs until number of errors is zero
}

static bool GLLogCall() 
{
    //While there is errors write them out 
    while (GLenum error = glGetError()) 
    {
        std::cout<< "[OpenGL Error] (" << error << ")" << std::endl;
        return false;
    }
    return true;
}


struct ShaderProgramSource 
{
    std::string VertexSource;
    std::string FragmentSource; 
};

//Reads in from single file fragment and vertex shader source code returns struct
static ShaderProgramSource ParseShader(const std::string& filepath) 
{
    //stream in the file from the file path
    std::ifstream stream(filepath);
    
    //ShaderType values to check in while to add instructions to each shader type
    enum class ShaderType 
    {   
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    
    //varible for each line of the file 
    std::string line;
   
    //vaiable for adding to our respective shaders
    std::stringstream ss[2];
    
    //Set our shader type none as def. 
    ShaderType type = ShaderType::NONE;

    // read in file line by line 
    while (getline(stream, line))
    {
        //read for #shader 
        if (line.find("#shader") != std::string::npos) 
        {
            if (line.find("vertex") != std::string::npos)
            {
                //set mode vertex
                type = ShaderType::VERTEX;

            }
            else if (line.find("fragment") != std::string::npos) 
            {
                //set mode fragment
                type = ShaderType::FRAGMENT;
            }
        }
        else 
        {
            //add the line to the string stream based on what type it is 
            ss[(int)type] << line << '\n';
        }

    }
    //We will return both things using a struct defined above
    return { ss[0].str(), ss[1].str() };
   
}
// compiles shader from source 
static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Error Handling 
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}
// takes in the vertex and fragment shaders and creates them
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return 1;
    }

    float positions[] = {
       -0.5f, -0.5f, //0
        0.5f, -0.5f, //1
        0.5f,  0.5f, //2
       -0.5f,  0.5f  //3
    };
    
    //Index Buffer- Dont want to redraw points 
    unsigned int indicies[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //Vertex Buufer is based on size of positions
    glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(float), positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    
    unsigned int ibo; //IndexBufferObject
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //Index Buufer is based on size of indicies no signed type
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW);


    //relative filepath name 
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);

    glBindBuffer(GL_ARRAY_BUFFER, 0);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //Type-num of verticies - type of data in inidcies - bound ibo above so pass in nullptr
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}