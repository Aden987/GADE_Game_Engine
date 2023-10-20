#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include "BasicMesh.h"
#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "basicCubeMesh.h"
#include "basicConeMesh.h"
#include "basicCylinderMesh.h"
#include "basicSphereMesh.h"
#include "HeightMapMesh.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int useWireframe = 0;
int displayGrayscale = 0;

//Learn OpenGL.[s.a.]. Welcome to OpenGL, [s.a.]. Available at: learnopengl.com [Accessed 17 September 2023].

//camera variables
glm::vec3 cameraPosition[3] = { glm::vec3(4.5f, 30.5f, 25.0f),
    glm::vec3(-4.5f, 22.0f, 17.0f),
    glm::vec3(4.5f, 30.5f, 4.5f),
};

float cameraYaw[3] = { 270.1f,-53.0f,270.0f };

float cameraPitch[3] = { -25.0f,-17.0f,-90.0f };

int cameraIndex = 0;
bool arrowKeyPressed = false;

Camera camera(cameraPosition[cameraIndex],
    glm::vec3(0.0f, 1.0f, 0.0f),
    cameraYaw[cameraIndex], cameraPitch[cameraIndex]);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
#pragma region GLFW_INIT_&_SETUP
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, 800, 600);
#pragma endregion

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader myShader("resources/shaders/basic.shader.vert","resources/shaders/basic.shader.frag");
    Shader heightMapShader("resources/shaders/heightmap.shader.vert", "resources/shaders/heightmap.shader.frag");

    //load texture
#pragma region TEXTURE
    unsigned int texture1;
    std::filesystem::path imagePath = "resources/textures/NewHeightMap.png";
    


    //texture 1
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    //texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //texture filtering parametres
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image,create textures, generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(imagePath.generic_string().c_str(), &width, &height, &nrChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // vertex generation for heightmap
    std::vector<float> hMapVertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    for (unsigned int i = 0; i < height; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            unsigned char* texel = data + (j + width * i) * nrChannels;
            unsigned char y = texel[0];

            hMapVertices.push_back(-height / 2.0f + i); //x
            hMapVertices.push_back((int)y * yScale - yShift); //y
            hMapVertices.push_back(-width/2.0f +j ); //z
        }
    }
    stbi_image_free(data);
#pragma endregion

    /*std::vector<BasicVertex> vertices = {
        {glm::vec3(-0.5f, -0.5f, 0.0f),glm::vec3(1.0f, 0.0f, 0.0f),glm::vec2(0.0f,0.0f)},
        {glm::vec3(0.5f, -0.5f, 0.0f),glm::vec3(0.0f, 1.0f, 0.0f),glm::vec2(1.0f,0.0f)},
        {glm::vec3(0.0f,  0.5f, 0.0f),glm::vec3(0.0f, 0.0f, 1.0f),glm::vec2(0.5f,1.0f)}
    };

    BasicMesh myTriangle(vertices);*/

    //index generation for height Map
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < height - 1; i++)
    {
        for (unsigned int j = 0; j < width; j++)
        {
            for (unsigned int k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k));
            }
        }
    }

    const unsigned int NUM_STRIPS = height - 1;
    const unsigned int NUM_VERTS_PER_STRIP = width * 2;
     
    //cube vertices
    std::vector<basicCubeVertex> vertices = {
        {glm::vec3(-0.5f, -0.5f, -0.5f) /*glm::vec2(0.0f, 0.0f)},*/},
        {glm::vec3(0.5f, -0.5f, -0.5f) /*glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(0.5f,  0.5f, -0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(0.5f,  0.5f, -0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(-0.5f,  0.5f, -0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(-0.5f, -0.5f, -0.5f)/* glm::vec2(0.0f, 0.0f)},*/},

        {glm::vec3(-0.5f, -0.5f,  0.5f)/* glm::vec2(0.0f, 0.0f)},*/},
        {glm::vec3(0.5f, -0.5f,  0.5f) /*glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(0.5f,  0.5f,  0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(0.5f,  0.5f,  0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(-0.5f,  0.5f,  0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(-0.5f, -0.5f,  0.5f) /*glm::vec2(0.0f, 0.0f)},*/},
        {glm::vec3(-0.5f,  0.5f,  0.5f)/* glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(-0.5f,  0.5f, -0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(-0.5f, -0.5f, -0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(-0.5f, -0.5f, -0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(-0.5f, -0.5f,  0.5f) /*glm::vec2(0.0f, 0.0f)},*/},
        {glm::vec3(-0.5f,  0.5f,  0.5f) /*glm::vec2(1.0f, 0.0f)},*/},

        {glm::vec3(0.5f,  0.5f,  0.5f) /*glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(0.5f,  0.5f, -0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(0.5f, -0.5f, -0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(0.5f, -0.5f, -0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(0.5f, -0.5f,  0.5f) /*glm::vec2(0.0f, 0.0f)},*/},
        {glm::vec3(0.5f,  0.5f,  0.5f) /*glm::vec2(1.0f, 0.0f)},*/},

        {glm::vec3(-0.5f, -0.5f, -0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(0.5f, -0.5f, -0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(0.5f, -0.5f,  0.5f) /*glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(0.5f, -0.5f,  0.5f) /*glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(-0.5f, -0.5f,  0.5f) /*glm::vec2(0.0f, 0.0f)},*/},
        {glm::vec3(-0.5f, -0.5f, -0.5f)/* glm::vec2(0.0f, 1.0f)},*/},

        {glm::vec3(-0.5f,  0.5f, -0.5f) /*glm::vec2(0.0f, 1.0f)},*/},
        {glm::vec3(0.5f,  0.5f, -0.5f) /*glm::vec2(1.0f, 1.0f)},*/},
        {glm::vec3(0.5f,  0.5f,  0.5f) /*glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(0.5f,  0.5f,  0.5f)/*glm::vec2(1.0f, 0.0f)},*/},
        {glm::vec3(-0.5f,  0.5f,  0.5f) /*glm::vec2(0.0f, 0.0f)},*/},
        {glm::vec3(-0.5f,  0.5f, -0.5f)/*, glm::vec2(0.0f, 1.0f)}*/}
    };

    basicCubeMesh myCube(vertices);

    //registering VAO for heightmap
    unsigned int terrainVAO, terrainVBO, terrainEBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, hMapVertices.size() * sizeof(float), &hMapVertices[0], GL_STATIC_DRAW);

    //pos attrin
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        Camera camera(cameraPosition[cameraIndex],
            glm::vec3(0.0f, 1.0f, 0.0f),
            cameraYaw[cameraIndex], cameraPitch[cameraIndex]);

        myShader.use();

        glBindTexture(GL_TEXTURE_2D, texture1);


        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);

#pragma region CHESSBOARD AND BORDER
        //Cube 
        /*myShader.setMat4("model", model);
        myCube.Draw(myShader);*/
        int chessBoardCol = 10;
        int chessBoardRow = 10;
        //standard height for chess board and border
        int boardHeight = 15;
        //randomized height for chess board
        int rndHeight;
        //maximum height for chess baord
        //int heightMaximum = 20;
        //for (unsigned int i = 0; i < chessBoardCol; i++)
        //{
        //    for (unsigned int j = 0; j < chessBoardRow; j++)
        //    {
        //        glm::mat4 model = glm::mat4(1.0f);
        //        //generates bottom border
        //        if (i == 0)
        //        {
        //            //checks if it is generating a corner piece of the border to adjust its scaling and position
        //            if (j == 0 || j == 9)
        //            {
        //                if (j == 0)
        //                {
        //                    model = glm::translate(model, glm::vec3(j + 0.3f, boardHeight, i + 0.3f));
        //                }
        //                else
        //                {
        //                    model = glm::translate(model, glm::vec3(j - 0.3f, boardHeight, i + 0.3f));
        //                }
        //                model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
        //            }
        //            else
        //            {
        //                model = glm::translate(model, glm::vec3(j, boardHeight, i + 0.3f));
        //                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.5f));
        //            }
        //            myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
        //        }
        //        //generate top border
        //        else if (i == 9)
        //        {
        //            if (j == 0 || j == 9)
        //            {
        //                if (j == 0)
        //                {
        //                    model = glm::translate(model, glm::vec3(j + 0.3f, boardHeight, i - 0.3f));
        //                }
        //                else
        //                {
        //                    model = glm::translate(model, glm::vec3(j - 0.3f, boardHeight, i - 0.3f));
        //                }
        //                model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
        //            }
        //            else
        //            {
        //                model = glm::translate(model, glm::vec3(j, boardHeight, i - 0.3f));
        //                model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.5f));
        //            }
        //            myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
        //        }
        //        else
        //        {
        //            //generates left border
        //            if (j == 0)
        //            {
        //                model = glm::translate(model, glm::vec3(j + 0.3f, boardHeight, i));
        //                model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
        //                myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
        //            }
        //            //generates right border
        //            else if (j == 9)
        //            {
        //                model = glm::translate(model, glm::vec3(j - 0.3f, boardHeight, i));
        //                model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
        //                myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
        //            }
        //            //generates chess board
        //            else
        //            {
        //                //rndHeight = rand() % (heightMaximum - boardHeight + 1) + boardHeight;
        //                model = glm::translate(model, glm::vec3(j, boardHeight, i));
        //                //checks if height is even to see if the chess row blocks need to alternate colour from the previous row
        //                if (i % 2 != 0)
        //                {
        //                    //non even row block is white
        //                    if (j % 2 != 0)
        //                    {
        //                        myShader.setVec3("colourIn", glm::vec3(255.0f, 255.0f, 255.0f));
        //                    }
        //                    //non even row block is black
        //                    else
        //                    {
        //                        myShader.setVec3("colourIn", glm::vec3(0.0f, 0.0f, 0.0f));
        //                    }
        //                }
        //                else
        //                {
        //                    if (j % 2 != 0)
        //                    {
        //                        myShader.setVec3("colourIn", glm::vec3(0.0f, 0.0f, 0.0f));
        //                    }
        //                    else
        //                    {
        //                        myShader.setVec3("colourIn", glm::vec3(255.0f, 255.0f, 255.0f));
        //                    }
        //                }
        //            }
        //        }
        //        myShader.setMat4("model", model);
        //        myCube.Draw(myShader);
        //    }
        //}
#pragma endregion

#pragma region HEIGHTMAP
        //heightMapShader.use();

        //// view/projection transformations for height map
        //glm::mat4 hMapProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        //glm::mat4 hMapView = camera.GetViewMatrix();
        //heightMapShader.setMat4("projection", hMapProjection);
        //heightMapShader.setMat4("view", hMapView);

        //// world transformation
        //glm::mat4 hMapModel = glm::mat4(1.0f);
        //heightMapShader.setMat4("model", hMapModel);

        //glBindVertexArray(terrainVAO);

        //for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
        //{
        //    glDrawElements(GL_TRIANGLE_STRIP, NUM_VERTS_PER_STRIP, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)* NUM_VERTS_PER_STRIP* strip));
        //}

#pragma endregion
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        cameraIndex++;
        if (cameraIndex > 2)
        {
            cameraIndex = 0;
        }
        //std::cout << cameraIndex << std::endl;
    }
    /* else
     {
         arrowKeyPressed = false;
     }*/
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        cameraIndex--;
        if (cameraIndex < 0)
        {
            cameraIndex = 2;
        }
        //std::cout << cameraIndex << std::endl;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_SPACE:
            useWireframe = 1 - useWireframe;
            break;
        case GLFW_KEY_G:
            displayGrayscale = 1 - displayGrayscale;
            break;
        default:
            break;
        }
    }
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}