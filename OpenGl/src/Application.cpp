#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"

#include <iostream>

#include "Renderer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "texture.h"


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
        return 1;
    }

    std::cout << "Using GL Version: " << glGetString(GL_VERSION) << std::endl;
   
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        // Problem: glewInit failed
        std::cout << "glewInit failed: " << glewGetErrorString(err) << std::endl;
        return 1;
    }
    //Scope to terminate program when screen is exited 
    {
        glfwSwapInterval(1);

        float positions[] = {
           -0.5f, -0.5f, 0.0f, 0.0f, //0
            0.5f, -0.5f, 1.0f, 0.0f, //1
            0.5f,  0.5f, 1.0f, 1.0f, //2
           -0.5f,  0.5f, 0.0f, 1.0f  //3
        };

        //Index Buffer- Dont want to redraw points 
        unsigned int indicies[] = {
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        //Vertex Array Object
        VertexArray va;
        VertexBuffer vb(positions, 4 * 4 * sizeof(float));

        VertexBufferLayout layout;
        layout.AddFloat(2);
        layout.AddFloat(2);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indicies, 6);

        glm::mat4 proj = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f);

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUnifrom4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);
        shader.SetUniformMat4f("u_MVP", proj);

        Texture texture("res/Textures/ComicBoom.png");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        //UnBind Vertex & Index Buffer
        va.UnBind();
        shader.UnBind();
        vb.UnBind();
        ib.UnBind();

        Renderer renderer;

        float r = 0.0f;
        float increment = 0.05f;

        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            shader.Bind();
            shader.SetUnifrom4f("u_Color", r, 0.3f, 0.7f, 1.0f);

            va.Bind();
            ib.Bind();
            renderer.Draw(va, ib, shader);
            
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
    }
    glfwTerminate();
    return 0;
}