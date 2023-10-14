#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <math.h>
#include "transform.h"
#include "house.h"
#include "star.h"
#include "circle.h"
#include "figure.h"

/* Includes for usb compiler

#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

*/

/*
    Curso: Computación gráfica
    Alumno: Bruno Raul Fernandez Gutierrez
*/

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//Settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

//Object params
float scale_range = 0.9f;
int count = 0;
float percent = 1.0f;
float defpercent = 0.01f;

//Vertex shader code
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

//Fragment shader code
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = ourColor;\n"
"}\n\0";

//Mode to render the circle
int polymode = GL_TRIANGLES;

//Number of poits for sides of circle
bool lock_s = false;

//Move locks
bool mov_s = false;

//Enter key lock
bool lock_add = false;

//Star functions
void updateStarVector(vec4& vec, float speed);
void moveStar(star& stary, vec4 vec);

//Limits for object
float lmt_right_star;
float lmt_left_star;
float lmt_up_star;
float lmt_down_star;

char dir_star = 'l';

int main()
{
    //Initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    //Creating a window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);

    //Checking if window was created succesfully
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    //Making the window the current context for thread execution
    glfwMakeContextCurrent(window);

    //Call back function when resizing the window
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    /*
    For vs: !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)
    For usb compiler: !gladLoadGL(glfwGetProcAddress)
    */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //Vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //Fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //Check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //Creating program and ID as return value
    unsigned int shaderProgram = glCreateProgram();

    //Attach both shaders compiled previously and link the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    //Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    //Dont need shader objects after linking them
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<vec4> tri_vtx = {
        vec4(-0.5f, -0.5f, 0.0f, 1.0f),
        vec4(0.5f, -0.5f, 0.0f, 1.0f),
        vec4(0.0f, 0.5f, 0.0f, 1.0f),
    };

    std::vector<unsigned int> tri_idx = {
        0, 1, 2
    };

    figure triangle(0.0f, 0.0f, 0.0f, tri_vtx, tri_idx);

    //Size of points and lines
    glPointSize(5.0f);
    glLineWidth(3.0f);

    float speed = 0.01f;

    vec4 trans_star(-speed, 0.0f, 0.0f, 0.0f);

    int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

    while (!glfwWindowShouldClose(window))
    {
        //Input
        processInput(window);

        //Chosing color to clear the screen with
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //Using shader program
        glUseProgram(shaderProgram);

        //Polygon mode for rendering
        glPolygonMode(GL_FRONT_AND_BACK, GL_TRIANGLES);

        //Normal before transformation
        vec4 v1 = triangle.vtx[1] - triangle.vtx[0];
        vec4 v2 = triangle.vtx[2] - triangle.vtx[0];

        vec4 normal = v1.cross_product(v2);
        normal = normal / sqrt(normal.dot(normal));

        std::cout << "Normal: " << std::endl;
        normal.print();

        triangle.draw(vertexColorLocation, GL_TRIANGLES, vec4(0.0f, 1.0f, 0.0f, 1.0f));

        //glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteProgram(shaderProgram);

    //glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


//Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //Drawing modes

    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        polymode = GL_LINE_LOOP;

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        polymode = GL_POINTS;

    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        polymode = GL_TRIANGLES;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        polymode = GL_LINE_STRIP;

    //Moving locks
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !lock_s)
    {
        lock_s = true;
        mov_s = !mov_s;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !lock_add)
    {
        lock_add = true;
    }

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) { lock_s = false; }
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) { lock_add = false; }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void updateStarVector(vec4& vec, float speed)
{
    float limit = 0.8f;
    if (lmt_right_star > limit && dir_star == 'r')
    {
        vec.update(0.0f, -speed, 0.0f, 0.0f);
        dir_star = 'd';
    }

    if (lmt_up_star > limit && dir_star == 'u')
    {
        vec.update(speed, 0.0f, 0.0f, 0.0f);
        dir_star = 'r';
    }

    if (lmt_left_star < -limit && dir_star == 'l')
    {
        vec.update(0.0f, speed, 0.0f, 0.0f);
        dir_star = 'u';
    }

    if (lmt_down_star < -limit && dir_star == 'd')
    {
        vec.update(-speed, 0.0f, 0.0f, 0.0f);
        dir_star = 'l';
    }
}

void moveStar(star& stary, vec4 vec)
{
    //stary.translate(vec);
    //stary.rotate(0.1f);


    if (percent > 1.0f + scale_range) { defpercent *= -1; }
    if (percent < 1.0f - scale_range) { defpercent *= -1; }
    percent += defpercent;
    stary.scale(percent, percent, percent);

    stary.print_vtx();

    lmt_right_star = stary.get_limit('r');
    lmt_left_star = stary.get_limit('l');

    lmt_up_star = stary.get_limit('u');
    lmt_down_star = stary.get_limit('d');
}

void updateIndicesLoop(star& stary)
{
    if (polymode == GL_LINE_LOOP)
    {
        stary.redo_indices("outline");
    }
    else
    {
        stary.redo_indices("triangle");
    }
}

