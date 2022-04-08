#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include<fstream>
#include<cstdio>
#include<iostream>
#include<string>
#include<sstream>
#include<intrin.h>

//Error Checking for all GL functions this is wrapped by should throough breakpoint but it isn't TODO thing
//For now will just return function file line 
#define ASSERT(x) if (!(x)) __debugbreak();

#define GLCall(x) GLCLearError();\
    x;\
    ASSERT(GLLogCall(#x, __FILE__,__LINE__))


static void GLCLearError()
{
    //loop to run until all errors are cleared
    while (glGetError() != GL_NO_ERROR);
    //runs until number of errors is zero
}

static bool GLLogCall(const char* function, const char* file, int line) 
{
    //While there is errors write them out 
    while (GLenum error = glGetError()) 
    {
        std::cout<< "[OpenGL Error] (" << error << "): " << function << " " << file <<": "<< line<< std::endl;
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
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    //Error Handling fro shader source code 
    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader: " << message << std::endl;
        GLCall(glDeleteShader(id));
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

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
    {
        std::cout << "Failed to Initialize lib." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(1024, 768, "PRACTICE", NULL, NULL);
    if (window == NULL) 
    {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) 
    {
        std::cout << "Failed to Initialize GLEW " << std::endl;
        return -1;
    }

    std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;
    
    //Sync gl with computers framerate
    glfwSwapInterval(1);

    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed
        std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
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
    
    //Vertex Array Object
    unsigned int vao;
    GLCall(glGenVertexArrays(1, &vao));
    GLCall(glBindVertexArray(vao));

    unsigned int buffer;
    GLCall(glGenBuffers(1, &buffer));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
    //Vertex Buufer is based on size of positions
    GLCall(glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), positions, GL_STATIC_DRAW));

    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
    
    //IndexBufferObject
    unsigned int ibo; 
    GLCall(glGenBuffers(1, &ibo));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
    //Index Buufer is based on size of indicies no signed type
    GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indicies, GL_STATIC_DRAW));


    //relative filepath name 
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    GLCall(glUseProgram(shader));

    //Get location of color code in shader file 
    GLCall(int location = glGetUniformLocation(shader, "u_Color"));
    //if location equals -1 no location on uniform
    ASSERT(location != -1);
    GLCall(glUniform4f(location, 0.8f, 0.3f, 0.7f, 1.0f));



    //UnBind Vertex & INdex Buffer
    GLCall(glBindVertexArray(0));
    GLCall(glUseProgram(0));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

    float r = 0.0f;
    float increment = 0.05f;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        GLCall(glClear(GL_COLOR_BUFFER_BIT));
       
        GLCall(glUseProgram(shader));
        // uniforms must be per draw
        GLCall(glUniform4f(location, r, 0.3f, 0.8f, 1.0f));

        GLCall(glBindVertexArray(vao));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
      
        //Type-num of verticies - type of data in inidcies - bound ibo above so pass in nullptr
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

        if (r > 1.0f)
            increment = -0.05f;
        else if (r < 0.0f)
            increment = 0.05f;
        r += increment;
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}