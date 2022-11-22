#include "libs.h"
#include "Camera.h"

const unsigned int n = 10;
const unsigned int noPoints = 37;

glm::vec3 points[n * n * n * noPoints];

Vertex vertices[n*n*n*4 + 8] =
{
    //Position                    //Color                     //Textcoord
    glm::vec3(0.f, 1.f, 0.f),     glm::vec3(0.f, 1.f, 0.f),   glm::vec2(0.f, 0.f),
    glm::vec3(1.f, 1.f, 0.f),     glm::vec3(1.f, 0.f, 0.f),   glm::vec2(0.f, 1.f),
    glm::vec3(0.f, 0.f, 0.f),     glm::vec3(0.f, 0.f, 1.f),   glm::vec2(1.f, 0.f),
    glm::vec3(1.f, 0.f, 0.f),     glm::vec3(1.f, 1.f, 1.f),   glm::vec2(1.f, 1.f),
    glm::vec3(0.f, 1.f, 1.f),     glm::vec3(1.f, 1.f, 0.f),   glm::vec2(0.f, 0.f),
    glm::vec3(1.f, 1.f, 1.f),     glm::vec3(1.f, 0.f, 1.f),   glm::vec2(0.f, 1.f),
    glm::vec3(0.f, 0.f, 1.f),     glm::vec3(0.f, 1.f, 1.f),   glm::vec2(1.f, 0.f),
    glm::vec3(1.f, 0.f, 1.f),     glm::vec3(0.f, 0.f, 0.f),   glm::vec2(1.f, 1.f)
};
unsigned nrOfVertices = sizeof(vertices) / sizeof(Vertex);

GLuint indices[n*n*n*3*4*2 + 36] =
{
    0, 2, 1,
    1, 2, 3,
    2, 6, 3,
    3, 6, 7,
    4, 7, 6,
    4, 5, 7,
    4, 0, 1,
    4, 1, 5,
    1, 3, 5,
    5, 3, 7,
    0, 4, 2,
    4, 6, 2
};
unsigned nrOfIndices = sizeof(indices) / sizeof(GLuint);

float getRandomFloat(float to)
{
    return static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / to));
}

float grf()
{
    return getRandomFloat(1.f / n) - 1.f / n / 2;
}

glm::vec3 pointInTheMiddle(glm::vec3 p1, glm::vec3 p2)
{
    return glm::vec3(((p1.x + p2.x) / 2), ((p1.y + p2.y) / 2), ((p1.z + p2.z) / 2));
}

Camera camera;

//Delta time
float dt = 0.f;
float currTime = 0.f;
float lastTime = 0.f;

//Mouse input
double lastMouseX = 0.;
double lastMouseY = 0.;
double mouseX = 0.;
double mouseY = 0.;
double mouseOffsetX = 0.;
double mouseOffsetY = 0.;
bool firstMouse = true;

void updateDt()
{
    currTime = static_cast<float>(glfwGetTime());
    dt = currTime - lastTime;
    lastTime = currTime;
}

void updateMouse(GLFWwindow* window)
{
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouse)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }

    mouseOffsetX = mouseX - lastMouseX;
    mouseOffsetY = mouseY - lastMouseY;

    lastMouseX = mouseX;
    lastMouseY = mouseY;
}

void updateInput(GLFWwindow* window, glm::vec3& position, glm::vec3& rotation, glm::vec3& scale)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.updateKeyboardInput(dt, UP);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        camera.updateKeyboardInput(dt, DOWN);
    }
    /*camPosition.x += mouseOffsetX / 1000.;
    camPosition.y -= mouseOffsetY / 1000.;*/
}

bool checkPosition(glm::vec3 position)
{
    for (int i = 0; i < n * n * n * noPoints; i++)
    {
        if (glm::distance(position, points[i]) < 0.01f) return false;
    }
    return true;
}

void framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
    glViewport(0, 0, fbW, fbH);
}

bool loadShaders(GLuint& program)
{
    bool loadSuccess = true;
    char infoLog[512];
    GLint success;

    std::string temp = "";
    std::string src = "";

    std::ifstream in_file;

    // VERTEX

    in_file.open("vertex_core.glsl");

    if (in_file.is_open())
    {
        while (std::getline(in_file, temp))
        {
            src += temp + "\n";
        }
    }
    else
    {
        std::cout << "ERROR:LOADSHADERS::VERTEX_FILE_OPENING" << "\n";
        loadSuccess = false;
    }
    in_file.close();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const GLchar* vertSrc = src.c_str();
    glShaderSource(vertexShader, 1, &vertSrc, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::LOADSHADERS::COULD_NOT_COMPILE_VERTEX_SHADER" << "\n";
        std::cout << infoLog << "\n";
        loadSuccess = false;
    }

    temp = "";
    src = "";

    // FRAGMENT

    in_file.open("fragment_core.glsl");

    if (in_file.is_open())
    {
        while (std::getline(in_file, temp))
        {
            src += temp + "\n";
        }
    }
    else
    {
        std::cout << "ERROR:LOADSHADERS::FRAGMENT_FILE_OPENING" << "\n";
        loadSuccess = false;
    }
    in_file.close();

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const GLchar* fragSrc = src.c_str();
    glShaderSource(fragmentShader, 1, &fragSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::LOADSHADERS::COULD_NOT_COMPILE_FRAGMENT_SHADER" << "\n";
        std::cout << infoLog << "\n";
        loadSuccess = false;
    }

    //PROGRAM
    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::LOADSHADERS::COULD_NOT_LINK_PROGRAM" << "\n";
        std::cout << infoLog << "\n";
        loadSuccess = false;
    }

    // END
    glUseProgram(0);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return loadSuccess;
}

int main(void)
{
    srand(time(NULL));
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    const int WINDOW_WIDTH = 1000;
    const int WINDOW_HEIGHT = 1000;
    int frameBufferWidth = 0;
    int frameBufferHeight = 0;

    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Zadanie 4", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

	// glViewport(0, 0, frameBufferWidth, frameBufferHeight);

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* GLEW */

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAILED" << "\n";
        glfwTerminate();
    }

    //OPENGL OPTIONS
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC0_ALPHA, GL_ONE_MINUS_SRC1_ALPHA);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    //SHADER INIT
    GLuint core_program;
    if (!loadShaders(core_program))
    {
        glfwTerminate();
    }
    //MODEL

    //GENERATION
    int counterV = 8;
    int counterI = 36;
    int pId = 0;
    float to = 1.f / n;
    for (float i = 0.05f; i < 0.96f; i += to)
    {
        for (float j = 0.05f; j < 0.96f; j += to)
        {
            for (float k = 0.05f; k < 0.96f; k += to)
            {
                vertices[counterV++] = {
                    glm::vec3(i + grf(), j + grf(), k + grf()),     glm::vec3(i, j, k),   glm::vec2(0.f, 0.f)
                };
                points[pId++] = glm::vec3(vertices[counterV - 1].position); // 0
                vertices[counterV++] = {
                    glm::vec3(i + grf(), j + grf(), k + grf()),     glm::vec3(i, j, k),   glm::vec2(0.f, 0.f)
                };
                points[pId++] = glm::vec3(vertices[counterV - 1].position); // 1
                vertices[counterV++] = {
                    glm::vec3(i + grf(), j + grf(), k + grf()),     glm::vec3(i, j, k),   glm::vec2(0.f, 0.f)
                };
                points[pId++] = glm::vec3(vertices[counterV - 1].position); // 2
                vertices[counterV++] = {
                    glm::vec3(i + grf(), j + grf(), k + grf()),     glm::vec3(i, j, k),   glm::vec2(0.f, 0.f)
                };
                points[pId++] = glm::vec3(vertices[counterV - 1].position); // 3

                points[pId++] = pointInTheMiddle(points[pId - 4], points[pId - 3]); // 4   0-1
                points[pId++] = pointInTheMiddle(points[pId - 5], points[pId - 1]); // 5   0-4
                points[pId++] = pointInTheMiddle(points[pId - 2], points[pId - 5]); // 6   4-1

                points[pId++] = pointInTheMiddle(points[pId - 6], points[pId - 5]); // 7   1-2
                points[pId++] = pointInTheMiddle(points[pId - 7], points[pId - 1]); // 8   1-7
                points[pId++] = pointInTheMiddle(points[pId - 7], points[pId - 2]); // 9   2-7

                points[pId++] = pointInTheMiddle(points[pId - 10], points[pId - 8]); // 10   0-2
                points[pId++] = pointInTheMiddle(points[pId - 11], points[pId - 1]); // 11   0-10
                points[pId++] = pointInTheMiddle(points[pId - 10], points[pId - 2]); // 12   2-10

                points[pId++] = pointInTheMiddle(points[pId - 13], points[pId - 10]); // 13   0-3
                points[pId++] = pointInTheMiddle(points[pId - 14], points[pId - 1]); // 14   0-13
                points[pId++] = pointInTheMiddle(points[pId - 12], points[pId - 2]); // 15   3-13

                points[pId++] = pointInTheMiddle(points[pId - 15], points[pId - 13]); // 16   1-3
                points[pId++] = pointInTheMiddle(points[pId - 16], points[pId - 1]); // 17   1-16
                points[pId++] = pointInTheMiddle(points[pId - 2], points[pId - 15]); // 18   16-3

                points[pId++] = pointInTheMiddle(points[pId - 17], points[pId - 16]); // 19   2-3
                points[pId++] = pointInTheMiddle(points[pId - 18], points[pId - 1]); // 20   2-19
                points[pId++] = pointInTheMiddle(points[pId - 18], points[pId - 2]); // 21   3-19

                points[pId++] = pointInTheMiddle(points[pId - 18], points[pId - 15]); // 22   4-7
                points[pId++] = pointInTheMiddle(points[pId - 19], points[pId - 13]); // 23   4-10
                points[pId++] = pointInTheMiddle(points[pId - 20], points[pId - 11]); // 24   4-13
                points[pId++] = pointInTheMiddle(points[pId - 21], points[pId - 9]); // 25   4-16
                points[pId++] = pointInTheMiddle(points[pId - 22], points[pId - 7]); // 26   4-19

                points[pId++] = pointInTheMiddle(points[pId - 20], points[pId - 17]); // 27   7-10
                points[pId++] = pointInTheMiddle(points[pId - 21], points[pId - 15]); // 28   7-13
                points[pId++] = pointInTheMiddle(points[pId - 22], points[pId - 13]); // 29   7-16
                points[pId++] = pointInTheMiddle(points[pId - 23], points[pId - 11]); // 30   7-19

                points[pId++] = pointInTheMiddle(points[pId - 21], points[pId - 18]); // 31   10-13
                points[pId++] = pointInTheMiddle(points[pId - 22], points[pId - 16]); // 32   10-16
                points[pId++] = pointInTheMiddle(points[pId - 23], points[pId - 14]); // 33   10-19

                points[pId++] = pointInTheMiddle(points[pId - 21], points[pId - 18]); // 34   13-16
                points[pId++] = pointInTheMiddle(points[pId - 22], points[pId - 16]); // 35   13-19

                points[pId++] = pointInTheMiddle(points[pId - 20], points[pId - 17]); // 36   16-19

                indices[counterI++] = counterV - 4;
                indices[counterI++] = counterV - 3;
                indices[counterI++] = counterV - 2;

                indices[counterI++] = counterV - 4;
                indices[counterI++] = counterV - 2;
                indices[counterI++] = counterV - 3;

                indices[counterI++] = counterV - 4;
                indices[counterI++] = counterV - 3;
                indices[counterI++] = counterV - 1;

                indices[counterI++] = counterV - 4;
                indices[counterI++] = counterV - 1;
                indices[counterI++] = counterV - 3;

                indices[counterI++] = counterV - 4;
                indices[counterI++] = counterV - 2;
                indices[counterI++] = counterV - 1;

                indices[counterI++] = counterV - 4;
                indices[counterI++] = counterV - 1;
                indices[counterI++] = counterV - 2;

                indices[counterI++] = counterV - 2;
                indices[counterI++] = counterV - 3;
                indices[counterI++] = counterV - 1;

                indices[counterI++] = counterV - 2;
                indices[counterI++] = counterV - 1;
                indices[counterI++] = counterV - 3;
            }
        }
    }

    //VAO, VBO, EBO
    GLuint VAO;
    glCreateVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //GEN VBO AND BING AND SEND DATA
    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // SET TO DYNAMIC WHEN MORE MOVEMENT

    //GEN EBO AND BIND AND SEND DATA
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // SET TO DYNAMIC WHEN MORE MOVEMENT

    //SET VERTEXATTRIBUTEPOINTERS AND ENABLE (INPUT ASSEMBLY) -- 
    //POS
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    //COLOR
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    //TEXCOOR
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, texcoord));
    glEnableVertexAttribArray(2);

    //BIND VAO 0
    glBindVertexArray(0);

    //CAMERA
    camera = Camera(glm::vec3(0.01f, 0.01f, 0.01f), glm::vec3(0.f, 0.f, 0.05f), glm::vec3(0.f, 1.f, 0.f));


    //INIT MATRICES
    glm::vec3 position(0.f);
    glm::vec3 rotation(0.f);
    glm::vec3 scale(1.f);

    glm::mat4 ModelMatrix(1.f);
    ModelMatrix = glm::translate(ModelMatrix, position);
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
    ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
    ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scale));

    /*glm::vec3 camPosition(0.f, 0.f, 2.f);
    glm::vec3 worldUp(0.f, 1.f, 0.f);
    glm::vec3 camFront(0.f, 0.f, -1.f);
    glm::mat4 ViewMatrix(1.f);
    ViewMatrix = glm::lookAt(camPosition, camPosition + camFront, worldUp);*/
    glm::mat4 ViewMatrix = camera.getViewMatrix();

    float fov = 90.f;
    float nearPlane = 0.0001f;
    float farPlane = 1000.f;
    glm::mat4 ProjectionMatrix(1.f);
    ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(frameBufferWidth) / frameBufferHeight, nearPlane, farPlane);

    //INIT MAT UNIFORMS
    glUseProgram(core_program);

    glUniformMatrix4fv(glGetUniformLocation(core_program, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    glUniformMatrix4fv(glGetUniformLocation(core_program, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));
    glUniformMatrix4fv(glGetUniformLocation(core_program, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));

    glUseProgram(0);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        glfwPollEvents();

        //UPDATE
        updateDt();
        updateMouse(window);
        glm::vec3 oldPos = camera.getPosition();
        updateInput(window, position, rotation, scale);
        if (!checkPosition(camera.getPosition())) camera.setPosition(oldPos);
        camera.updateInput(dt, -1, mouseOffsetX, mouseOffsetY);

        glm::vec3 pos = camera.getPosition();
        if (pos.x > 0.985f && pos.y > 0.985f && pos.z > 0.985f)
        {
            std::cout << "YOU WON!" << std::endl;
        }
        //DRAW -- 


        //CLEAR
        glClearColor(0.5f, 0.f, 0.5f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //USE A PROGRAM
        glUseProgram(core_program);

        //MOVE ROTATE SCALE
        /*rotation.x += 0.5f;
        rotation.y += 0.5f;
        rotation.z += 0.5f;*/


        ModelMatrix = glm::mat4(1.f);
        ModelMatrix = glm::translate(ModelMatrix, position);
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f));
        ModelMatrix = glm::rotate(ModelMatrix, glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        ModelMatrix = glm::scale(ModelMatrix, glm::vec3(scale));

        ViewMatrix = camera.getViewMatrix();

        //UPDATE UNIFORM
        glUniformMatrix4fv(glGetUniformLocation(core_program, "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        glUniformMatrix4fv(glGetUniformLocation(core_program, "ViewMatrix"), 1, GL_FALSE, glm::value_ptr(ViewMatrix));

        int frameBufferWidthCopy = frameBufferWidth;
        int frameBufferHeightCopy = frameBufferHeight;
        glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

        if (frameBufferWidth != frameBufferWidthCopy || frameBufferHeight != frameBufferHeightCopy)
        {
            ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(frameBufferWidth) / frameBufferHeight, nearPlane, farPlane);
            glUniformMatrix4fv(glGetUniformLocation(core_program, "ProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(ProjectionMatrix));
        }

        //BIND VERTEX ARRAY
        glBindVertexArray(VAO);

        //DRAW
        //glDrawArrays(GL_TRIANGLES, 0, nrOfVertices);
        glDrawElements(GL_TRIANGLES, nrOfIndices, GL_UNSIGNED_INT, 0);

        //SWAP BUFFERS / END
        glfwSwapBuffers(window);
        glFlush();
    }

    /* END */
    glfwDestroyWindow(window);
    glfwTerminate();

    glDeleteProgram(core_program);

    return 0;
}