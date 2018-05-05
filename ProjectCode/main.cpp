//GLAD
//#include "Glitter/Vendor/glad/include/glad/glad.h"
//#include "Glitter\Vendor\glad\src\glad.c"

//GLFW
//#include "/Users/krishnamarentes/glfw-3.3/include/GLFW/glfw3.h"

#include <iostream>
#include <stdio.h>

#include "Object.h"
//#include "Object.cpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource = "#version 150\n"
"in vec4 vPosition;\n"
"in vec4 vColor;\n"
"out vec4 color;\n"
"uniform vec3 theta;\n" 
"void main()\n"
"{\n"
"   //Compute the sines and cosines of theta for each of\n"
"   //the three axes in one computation.\n"
"   vec3 angles = radians(theta);\n"
"   vec3 c = cos(angles);\n"
"   vec3 s = sin(angles);\n"
"   //Remember: these matrices are column major.\n"
"   mat4 rx = mat4(1.0, 0.0, 0.0, 0.0,\n"
"                  0.0, c.x, s.x, 0.0,\n"
"                  0.0, -s.x, c.x, 0.0,\n"
"                  0.0, 0.0, 0.0, 1.0);\n"
"   mat4 ry = mat4(c.y, 0.0, -s.y, 0.0,\n"
"                  0.0, 1.0, 0.0, 0.0,\n"
"                  s.y, 0.0, c.y, 0.0,\n"
"                  0.0, 0.0, 0.0, 1.0);\n"
"   mat4 rz = mat4(c.z, -s.z, 0.0, 0.0,\n"
"                  s.z, c.z, 0.0, 0.0,\n"
"                  0.0, 0.0, 1.0, 0.0,\n"
"                  0.0, 0.0, 0.0, 1.0);\n"
"   color = vColor;\n"
"   gl_Position = rz * ry * rx * vPosition;\n"
"}\0";

const char *fragmentShaderSource = "#version 150\n"
"in vec4 color;\n"
"out vec4 fColor;\n"
"void main()\n"
"{\n"
"   fColor = color;\n"
"}\n\0";

//GLOBALS
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int Axis = Xaxis;

GLfloat Theta[NumAxes] = { 0.0, 0.0, 0.0 };

GLint theta;

void init(GLuint* VAO, int num_models, Object *obj)
{
    
    int success;
    int i;
    
    char infoLog[512];
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GLuint program = glCreateProgram();
    
    glGenVertexArrays(num_models, VAO);
    
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    glUseProgram(program);
    
    for (i = 0; i < num_models; i++) {
        glBindVertexArray(VAO[i]);
        obj[i].load(program);
    }
    
    glBindVertexArray(VAO[0]);
    theta = glGetUniformLocation(program, "theta");
    
    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    
}

void display(Object* obj, int selection)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUniform1fv(theta, 1, Theta);
    
    obj[selection].draw();
}

void idle() {
    Theta[Axis] += 10.0; // might try using 0.01 intervals
    
    if (Theta[Axis] > 360.0) {
        Theta[Axis] -= 360.0;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods, int selection, int num_models, GLuint* VAO)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        exit(EXIT_SUCCESS);
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        selection = (selection + 1) % num_models;
        glBindVertexArray(VAO[selection]);
    }
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        Axis = Zaxis;
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        Axis = Xaxis;
    if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
        Axis = Yaxis;
}

int main() {
    
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); //added

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // goes here
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    
    //printf("%s\n",glfwGetVersionString());
    
    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Marentes_Hoffstuen Object Loading", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    GLuint VAO[2];
    
    Object obj[] = {
        //Object("../../../Blender & Obj files/obj files/Cube.obj"),
        //Object("../../../Blender & Obj files/obj files/teapot.obj")
        Object("Glitter/Sources/Cube.obj"),
        Object("Glitter/Sources/teapot.obj")
    };
    
    init(VAO, 2, obj);
    
    //glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    while (!glfwWindowShouldClose(window)) {
        glfwSwapBuffers(window);
        display(obj, 0);
        idle();
        glfwPollEvents();
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources
    // -----------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        static bool wireframe_mode = false;
        if (wireframe_mode == false) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            wireframe_mode = true;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            wireframe_mode = false;
        }
    }
}

//glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    
    
}
