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
#include "ObjectContainer.h"
#include "AnimationController.cpp"
#include "model.h"
#include "lighting.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadCubemap(vector<std::filesystem::path> faces);

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
bool rightArrowKeyPressed = false;
bool leftArrowKeyPressed = false;
bool spaceKeyPressed = false;
bool camKeyPressed = false;
bool camSwitch = false;
bool animPlay = false;

Camera camera(cameraPosition[cameraIndex],
    glm::vec3(0.0f, 1.0f, 0.0f),
    cameraYaw[cameraIndex], cameraPitch[cameraIndex]);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  18.0f,  2.0f),
        glm::vec3(2.3f,  12.0f, 4.0f),
        glm::vec3(4.0f,   18.0f, 10.0f),
        glm::vec3(9.0f,  18.0f, 3.0f)
};

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
    Shader importShader("resources/shaders/importBasic.shader.vert", "resources/shaders/importBasic.shader.frag");
    Shader skyboxShader("resources/shaders/skybox.shader.vert", "resources/shaders/skybox.shader.frag");
    Shader lightSourceShader("resources/shaders/lightSource.shader.vert", "resources/shaders/lightSource.shader.frag");
    Shader lightingShader("resources/shaders/lighting.shader.vert", "resources/shaders/lighting.shader.frag");

    //load texture
#pragma region TEXTURE
    unsigned int texture2;
    unsigned int texture3;
    unsigned int texture4;
    //white piece
    unsigned int texture5;
    //brown piece
    unsigned int texture6;
    unsigned int texture7;

    unsigned int texType;

    std::filesystem::path imagePath2 = "resources/textures/blackTexture.png";
    std::filesystem::path imagePath3 = "resources/textures/whiteBlock.png";
    std::filesystem::path imagePath4 = "resources/textures/wood.png";
    std::filesystem::path imagePath5 = "resources/textures/whitePiece.png";
    std::filesystem::path imagePath6 = "resources/textures/brownPiece.png";
    std::filesystem::path imagePath7 = "resources/textures/sandtex.png";
    
    //glUniform1i(glGetUniformLocation(myShader.ID, "texture1"), 0);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    // load image, create texture and generate mipmaps
    unsigned char* data = stbi_load(imagePath2.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //glUniform1i(glGetUniformLocation(myShader.ID, "texture2"), 1);
    //// texture 3
    //// ---------
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(imagePath3.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //texture 4
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(imagePath4.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //texture 5
    glGenTextures(1, &texture5);
    glBindTexture(GL_TEXTURE_2D, texture5);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(imagePath5.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //texture 6
    glGenTextures(1, &texture6);
    glBindTexture(GL_TEXTURE_2D, texture6);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(imagePath6.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //texture7
    glGenTextures(1, &texture7);
    glBindTexture(GL_TEXTURE_2D, texture7);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(imagePath7.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    myShader.setInt("texture2", 1);
    myShader.setInt("texture3", 2);
    myShader.setInt("texture4", 3);
    myShader.setInt("texture5", 4);
    myShader.setInt("texture6", 5);
    myShader.setInt("texture7", 6);
#pragma endregion

    //skybox texture
    std::filesystem::path rightMap = "resources/textures/skybox/right.jpg";
    std::filesystem::path leftMap = "resources/textures/skybox/left.jpg";
    std::filesystem::path topMap = "resources/textures/skybox/top.jpg";
    std::filesystem::path bottomMap = "resources/textures/skybox/bottom.jpg";
    std::filesystem::path frontMap = "resources/textures/skybox/front.jpg";
    std::filesystem::path backMap = "resources/textures/skybox/back.jpg";

    /*std::filesystem::path rightMap = "resources/textures/newSkybox/right.jpg";
    std::filesystem::path leftMap = "resources/textures/newSkybox/left.jpg";
    std::filesystem::path topMap = "resources/textures/newSkybox/top.jpg";
    std::filesystem::path bottomMap = "resources/textures/newSkybox/bottom.jpg";
    std::filesystem::path frontMap = "resources/textures/newSkybox/front.jpg";
    std::filesystem::path backMap = "resources/textures/newSkybox/back.jpg";*/

    vector<std::filesystem::path> faces
    {
        rightMap,
        leftMap,
        topMap,
        bottomMap,
        frontMap,
        backMap
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    //skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

#pragma region HeightMapTexture
    unsigned int texture1;

    std::filesystem::path imagePath = "resources/textures/NewHeightMap.png";
    
    //height map
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    //texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //texture filtering parametres
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image,create textures, generate mipmaps
    int width2, height2, nrChannels2;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data2 = stbi_load(imagePath.generic_string().c_str(), &width2, &height2, &nrChannels2, 0);
    if (data2)
    {
        GLenum format;
        if (nrChannels2 == 1)
            format = GL_RED;
        else if (nrChannels2 == 3)
            format = GL_RGB;
        else if (nrChannels2 == 4)
            format = GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width2, height2, 0, format, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }

    // vertex generation for heightmap
    std::vector<float> hMapVertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    for (unsigned int i = 0; i < height2; i++)
    {
        for (unsigned int j = 0; j < width2; j++)
        {
            unsigned char* texel = data2 + (j + width2 * i) * nrChannels2;
            unsigned char y = texel[0];

            hMapVertices.push_back(-height2 / 2.0f + i); //x
            hMapVertices.push_back((int)y * yScale - yShift); //y
            hMapVertices.push_back(-width2 / 2.0f + j); //z
        }
    }
    stbi_image_free(data2);
#pragma endregion

    //index generation for height Map
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < height2 - 1; i++)
    {
        for (unsigned int j = 0; j < width2; j++)
        {
            for (unsigned int k = 0; k < 2; k++)
            {
                indices.push_back(j + width2 * (i + k));
            }
        }
    }

    const unsigned int NUM_STRIPS = height2 - 1;
    const unsigned int NUM_VERTS_PER_STRIP = width2 * 2;
     
    //creating primitive shapes
    //cube vertices
   /* std::vector<basicCubeVertex> vertices = {
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},

        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},

        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},

        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},

        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},

        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
    };*/
    std::vector<basicCubeVertex> vertices = {
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},

        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec2(0.0f, 0.0f)},

        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},

        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec2(1.0f, 0.0f)},

        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f),  glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},

        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f),  glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec2(0.0f, 1.0f)},
    };
    basicCubeMesh myCube(vertices);

    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    //cylinder model properties
    float cylinderRadius = 0.5f;
    float cylinderHeight = 1.0f;
    int cylinderSides = 16; // Adjust the number of sides as needed

    basicCylinderMesh myCylinder(cylinderRadius, cylinderHeight, cylinderSides);

    // cone model properties
    float coneRadius = 0.5f;
    float coneHeight = 1.0f;
    int coneSides = 16; // Adjust the number of sides as needed

    basicConeMesh myCone(coneRadius, coneHeight, coneSides);

    //sphere model
    float sphereRadius = 0.5f;
    int sphereSegments = 32; // Adjust the number of segments as needed

    basicSphereMesh mySphere(sphereRadius, sphereSegments);

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

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    AnimationController anim;

    std::filesystem::path modelPath = "resources/models/backpack/backpack.obj";
    Model ourModel(modelPath.generic_string().c_str());

    std::filesystem::path monolithPath = "resources/models/monolith/monolith.obj";
    Model monolithModel(monolithPath.generic_string().c_str());
    std::filesystem::path pyramidPath = "resources/models/pyramid/pyramid.obj";
    Model pyramidModel(pyramidPath.generic_string().c_str());
    std::filesystem::path templePath = "resources/models/temple/temple.obj";
    Model templeModel(templePath.generic_string().c_str());
    
    //counter to calculate fps
    unsigned int counter = 0;


    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        
        counter++;

        //Gets updated every 30th of a second
        if (deltaTime >= 1.0f / 30.0)
        {
            std::string FPS = std::to_string((1.0 / deltaTime) * counter);
            std::string ms = std::to_string((deltaTime / counter) * 1000);
            std::string newTitle = "LearnOpenGL Current FPS: " + FPS + "FPS/ " + ms + "ms";
            //set window title with newTitle
            glfwSetWindowTitle(window, newTitle.c_str());
            //get time diffrence back to 0
            lastFrame = currentFrame;
            //previousTime = currentTime;
            counter = 0;
        }

        anim.update(deltaTime);

        processInput(window);

        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (camSwitch == true)
        {
           /* Camera camera(cameraPosition[cameraIndex],
                glm::vec3(0.0f, 1.0f, 0.0f),
                cameraYaw[cameraIndex], cameraPitch[cameraIndex]);*/
            glfwSetCursorPosCallback(window, 0);
            glfwSetScrollCallback(window, 0);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            
        }
        else
        {
            Camera camera(cameraPosition[cameraIndex],
                glm::vec3(0.0f, 1.0f, 0.0f),
                cameraYaw[cameraIndex], cameraPitch[cameraIndex]);
            glfwSetCursorPosCallback(window, mouse_callback);
            glfwSetScrollCallback(window, scroll_callback);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 skyView = camera.GetViewMatrix();

        /*myShader.use();
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);*/

        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
        glm::vec3 lightColor = glm::vec3(1.0f);
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setInt("material.diffuse", 0);
        lightingShader.setInt("material.specular", 1);

        // directional light
        lightingShader.setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
        lightingShader.setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
        lightingShader.setVec3("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        // point light 1
        lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        lightingShader.setVec3("pointLights[0].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.setVec3("pointLights[0].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightingShader.setVec3("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.09f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
        // point light 2
        lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
        lightingShader.setVec3("pointLights[1].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.setVec3("pointLights[1].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightingShader.setVec3("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.09f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
        // point light 3
        lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
        lightingShader.setVec3("pointLights[2].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.setVec3("pointLights[2].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightingShader.setVec3("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("pointLights[2].constant", 1.0f);
        lightingShader.setFloat("pointLights[2].linear", 0.09f);
        lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
        // point light 4
        lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
        lightingShader.setVec3("pointLights[3].ambient", glm::vec3(0.05f, 0.05f, 0.05f));
        lightingShader.setVec3("pointLights[3].diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
        lightingShader.setVec3("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("pointLights[3].constant", 1.0f);
        lightingShader.setFloat("pointLights[3].linear", 0.09f);
        lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
        // spotLight
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
        lightingShader.setVec3("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setVec3("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        
        ObjectContainer pawnPiece;
        ObjectContainer rookPiece;
        ObjectContainer knightPiece;
        ObjectContainer bishopPiece;
        ObjectContainer queenPiece;
        ObjectContainer kingPiece;


#pragma region InstantiateChessPieces
        //Michael Chess Pieces into containers

        kingPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 3.0f, 0.8f));

        kingPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 1.8f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 4.0f));

        kingPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 3.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.5f, 2.5, 0.8f));

        kingPiece.addConeMesh(myCone,
            glm::vec3(0.0f, 0.0f, 2.9f),
            glm::vec3(180.0f, 0.0f, 0.0f),
            glm::vec3(2.5f, 2.5f, 4.0f));

        kingPiece.addConeMesh(myCone,
            glm::vec3(0.0f, 0.0f, 5.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.5f, 2.5f, 1.0f));

        kingPiece.addCubeMesh(myCube,
            glm::vec3(0.0f, 0.0f, 6.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.6f, 0.6f, 2.0f));

        kingPiece.addCubeMesh(myCube,
            glm::vec3(0.0f, 0.0f, 6.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 0.6f, 0.6f));


        queenPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 3.0f, 0.8f));

        queenPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 1.8f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 4.0f));

        queenPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 3.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.5f, 2.5, 0.8f));

        queenPiece.addConeMesh(myCone,
            glm::vec3(0.0f, 0.0f, 2.9f),
            glm::vec3(180.0f, 0.0f, 0.0f),
            glm::vec3(2.5f, 2.5f, 4.0f));

        queenPiece.addSphereMesh(mySphere,
            glm::vec3(0.0f, 0.0f, 4.4f),
            glm::vec3(180.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 2.0f));

        queenPiece.addSphereMesh(mySphere,
            glm::vec3(0.0f, 0.0f, 5.4f),
            glm::vec3(180.0f, 0.0f, 0.0f),
            glm::vec3(0.5f, 0.5f, 0.5f));


        bishopPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 3.0f, 0.8f));

        bishopPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 1.8f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 4.0f));

        bishopPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 3.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.5f, 2.5, 0.8f));

        bishopPiece.addSphereMesh(mySphere,
            glm::vec3(0.0f, 0.0f, 4.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 3.0f));

        bishopPiece.addSphereMesh(mySphere,
            glm::vec3(0.0f, 0.0f, 6.2f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 1.0f));


        rookPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 3.0f, 0.8f));

        rookPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 1.8f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 4.0f));

        rookPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 3.4f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.5f, 2.5, 0.8f));
         
        rookPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 3.9f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 3.0f, 2.0f));


        knightPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 3.0f, 0.8f));

        knightPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 0.5f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 2.0f));
        
        knightPiece.addCubeMesh(myCube,          
            glm::vec3(-0.5f, 0.0f, 2.0f),
            glm::vec3(0.0f, -30.0f, 0.0f),
            glm::vec3(1.5f, 0.5f, 3.0f));

        knightPiece.addCubeMesh(myCube,
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 50.0f, 0.0f),
            glm::vec3(1.5f, 0.5f, 4.0f));

        knightPiece.addCubeMesh(myCube,
            glm::vec3(1.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, -30.0f, 0.0f),
            glm::vec3(1.5f, 0.5f, 2.0f));


        pawnPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(3.0f, 3.0f, 0.8f));

        pawnPiece.addConeMesh(myCone,
            glm::vec3(0.0f, 0.0f, 1.8f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(2.0f, 2.0f, 4.0f));

        pawnPiece.addCylinderMesh(myCylinder,
            glm::vec3(0.0f, 0.0f, 3.5f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.5f, 1.5f, 0.5f));

        pawnPiece.addSphereMesh(mySphere,
            glm::vec3(0.0f, 0.0f, 4.3f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.5f, 1.5f, 1.5f));
#pragma endregion

#pragma region GenerateChessPieces
        //genrating chess pieces
        //spawn white pawns
        for (int i = 1; i <= 8; i++)
        {
            glm::mat4 modelPiece = glm::mat4(1.0f);
            modelPiece = glm::translate(modelPiece, glm::vec3(i, 16.0f, 7.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            glBindTexture(GL_TEXTURE_2D, texture5);

            modelPiece = glm::rotate(modelPiece, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            modelPiece = glm::translate(modelPiece, glm::vec3(0.0f, 0.0f, -1.0f));
            if (animPlay == true && i == 8)
            {
                anim.applyBouncingAnimation(modelPiece,1.0f,5.0f);
            }
            if (animPlay == true && i == 3)
            {
                //anim.applyBouncingAnimation(modelPiece,1.0f,5.0f);
                anim.applyPawnAnimation(modelPiece, 5.0f, 5.0f, 3.5f);
            }
            pawnPiece.Draw(modelPiece, lightingShader);
            /*myShader.setMat4("model", modelPiece);
            myCube.Draw(myShader);*/
        }

        //spawn black pawns
        for (int i = 1; i <= 8; i++)
        {
            glm::mat4 modelPiece = glm::mat4(1.0f);
            modelPiece = glm::translate(modelPiece, glm::vec3(i, 16.0f, 2.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            glBindTexture(GL_TEXTURE_2D, texture6);

            modelPiece = glm::rotate(modelPiece, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            modelPiece = glm::translate(modelPiece, glm::vec3(0.0f, 0.0f, -1.0f));
            if (animPlay == true && i == 1)
            {
                anim.applyRotationAnimation(modelPiece, 30.0f, glm::vec3(0.0f, 0.0f, 1.0f));
            }
            if (animPlay == true && i == 8)
            {
                 //anim.applyBouncingAnimation(modelPiece,1.0f,5.0f);
                anim.applyPawnAnimation(modelPiece, 5.0f, 5.0f, -3.5f);
            }
            pawnPiece.Draw(modelPiece, lightingShader);
            /*myShader.setMat4("model", modelPiece);
            myCube.Draw(myShader);*/
        }

        //spawn rooks
        for (int i = 0; i <= 3; i++)
        {
            glm::mat4 modelPiece = glm::mat4(1.0f);
            //white pieces
            if (i == 0)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(1.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
                if (animPlay == true)
                {
                    anim.applyRotationAnimation(modelPiece, 30.0f, glm::vec3(1.0f, 0.0f, 0.0f));
                }
            }
            else if (i == 1)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(8.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
            }
            //black pieces
            else if (i == 2)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(1.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
            }
            else
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(8.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
            }
            modelPiece = glm::rotate(modelPiece, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            modelPiece = glm::translate(modelPiece, glm::vec3(0.0f, 0.0f, -1.0f));            
            rookPiece.Draw(modelPiece, lightingShader);
        }

        //spawn knights
        for (int i = 0; i <= 3; i++)
        {
            glm::mat4 modelPiece = glm::mat4(1.0f);
            //white pieces
            if (i == 0)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(2.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
            }
            else if (i == 1)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(7.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
                if (animPlay == true)
                {
                    anim.applyKnightAnimation(modelPiece, 1.5f, 5.0f, -2.5f, -1.2f);
                }
            }
            //black pieces
            else if (i == 2)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(2.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
            }
            else
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(7.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
            }
            modelPiece = glm::rotate(modelPiece, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelPiece = glm::rotate(modelPiece, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            modelPiece = glm::translate(modelPiece, glm::vec3(0.0f, 0.0f, -1.0f));
            knightPiece.Draw(modelPiece, lightingShader);
        }

        //spawn bishops
        for (int i = 0; i <= 3; i++)
        {
            glm::mat4 modelPiece = glm::mat4(1.0f);
            //white pieces
            if (i == 0)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(3.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
            }
            else if (i == 1)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(6.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
            }
            //black pieces
            else if (i == 2)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(3.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
                if (animPlay == true)
                {
                    anim.applyYBouncingAnimation(modelPiece, 1.0f, 2.0f);
                }
            }
            else
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(6.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
                if (animPlay == true)
                {
                    anim.applyComplicatedAnimation(modelPiece);
                }
            }
            modelPiece = glm::rotate(modelPiece, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            modelPiece = glm::translate(modelPiece, glm::vec3(0.0f, 0.0f, -1.0f));
            bishopPiece.Draw(modelPiece, lightingShader);
        }

        //spawn queens
        for (int i = 0; i <= 1; i++)
        {
            glm::mat4 modelPiece = glm::mat4(1.0f);
            //white pieces
            if (i == 0)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(4.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
                if (animPlay == true)
                {
                    anim.applyBreathingAnimation(modelPiece);
                }
            }
            //black piece
            else
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(4.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
            }
            modelPiece = glm::rotate(modelPiece, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            modelPiece = glm::translate(modelPiece, glm::vec3(0.0f, 0.0f, -1.0f));
            queenPiece.Draw(modelPiece, lightingShader);
        }

        //spawn kings
        for (int i = 0; i <= 1; i++)
        {
            glm::mat4 modelPiece = glm::mat4(1.0f);
            //white pieces
            if (i == 0)
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(5.0f, 16.0f, 8.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture5);
            }
            //black piece
            else
            {
                modelPiece = glm::translate(modelPiece, glm::vec3(5.0f, 16.0f, 1.0f));
                modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, texture6);
                if (animPlay == true)
                {
                    anim.applyOffsetRotationAnimation(modelPiece, 15.0f, 0.0f);
                }
            }
            modelPiece = glm::rotate(modelPiece, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            modelPiece = glm::scale(modelPiece, glm::vec3(0.5f, 0.5f, 0.5f));
            modelPiece = glm::translate(modelPiece, glm::vec3(0.0f, 0.0f, -1.0f));
            kingPiece.Draw(modelPiece, lightingShader);
        }
#pragma endregion

#pragma region CHESSBOARD AND BORDER
        int chessBoardCol = 10;
        int chessBoardRow = 10;
        //standard height for chess board and border
        int boardHeight = 15;
        //randomized height for chess board
        int rndHeight;
        //maximum height for chess baord
        //int heightMaximum = 20;
        for (unsigned int i = 0; i < chessBoardCol; i++)
        {
            for (unsigned int j = 0; j < chessBoardRow; j++)
            {
                glm::mat4 model = glm::mat4(1.0f);
                //generates bottom border
                if (i == 0)
                {
                    //checks if it is generating a corner piece of the border to adjust its scaling and position
                    if (j == 0 || j == 9)
                    {
                        if (j == 0)
                        {
                            model = glm::translate(model, glm::vec3(j + 0.3f, boardHeight, i + 0.3f));
                        }
                        else
                        {
                            model = glm::translate(model, glm::vec3(j - 0.3f, boardHeight, i + 0.3f));
                        }
                        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
                    }
                    else
                    {
                        model = glm::translate(model, glm::vec3(j, boardHeight, i + 0.3f));
                        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.5f));
                    }
                    //myShader.setInt("texType", 1);
                    //glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, texture4);
                    //myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
                }
                //generate top border
                else if (i == 9)
                {
                    if (j == 0 || j == 9)
                    {
                        if (j == 0)
                        {
                            model = glm::translate(model, glm::vec3(j + 0.3f, boardHeight, i - 0.3f));
                        }
                        else
                        {
                            model = glm::translate(model, glm::vec3(j - 0.3f, boardHeight, i - 0.3f));
                        }
                        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
                    }
                    else
                    {
                        model = glm::translate(model, glm::vec3(j, boardHeight, i - 0.3f));
                        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 0.5f));
                    }
                    //myShader.setInt("texType", 1);
                    //glActiveTexture(GL_TEXTURE1);
                    glBindTexture(GL_TEXTURE_2D, texture4);
                    //myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
                }
                else
                {
                    //generates left border
                    if (j == 0)
                    {
                        model = glm::translate(model, glm::vec3(j + 0.3f, boardHeight, i));
                        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
                        //myShader.setInt("texType", 1);
                        //glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, texture4);
                        //myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
                    }
                    //generates right border
                    else if (j == 9)
                    {
                        model = glm::translate(model, glm::vec3(j - 0.3f, boardHeight, i));
                        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 1.0f));
                        //myShader.setInt("texType", 1);
                        //glActiveTexture(GL_TEXTURE1);
                        glBindTexture(GL_TEXTURE_2D, texture4);
                        //myShader.setVec3("colourIn", glm::vec3(123.0f, 63.0f, 0.0f));
                    }
                    //generates chess board
                    else
                    {
                        //rndHeight = rand() % (heightMaximum - boardHeight + 1) + boardHeight;
                        model = glm::translate(model, glm::vec3(j, boardHeight, i));
                        //checks if height is even to see if the chess row blocks need to alternate colour from the previous row
                        if (i % 2 != 0)
                        {
                            //non even row block is white
                            if (j % 2 != 0)
                            {
                                //myShader.setInt("texType", 2);
                                //glActiveTexture(GL_TEXTURE2);
                                glBindTexture(GL_TEXTURE_2D, texture3);
                                //myShader.setVec3("colourIn", glm::vec3(255.0f, 255.0f, 255.0f));
                            }
                            //non even row block is black
                            else
                            {
                                //myShader.setInt("texType", 1);
                                //glActiveTexture(GL_TEXTURE1);
                                glBindTexture(GL_TEXTURE_2D, texture2);
                                //myShader.setVec3("colourIn", glm::vec3(0.0f, 0.0f, 0.0f));
                            }
                        }
                        else
                        {
                            if (j % 2 != 0)
                            {
                                //myShader.setInt("texType", 1);
                                //glActiveTexture(GL_TEXTURE1);
                                glBindTexture(GL_TEXTURE_2D, texture2);
                                //myShader.setVec3("colourIn", glm::vec3(0.0f, 0.0f, 0.0f));
                            }
                            else
                            {
                                //myShader.setInt("texType", 2);
                                //glActiveTexture(GL_TEXTURE2);
                                glBindTexture(GL_TEXTURE_2D, texture3);
                                //myShader.setVec3("colourIn", glm::vec3(255.0f, 255.0f, 255.0f));
                            }
                        }
                    }
                }
                lightingShader.setMat4("model", model);
                myCube.Draw(lightingShader);
            }
        }
#pragma endregion

        lightSourceShader.use();
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);
        glm::mat4 lightSourceModel = glm::mat4(1.0f);
        for (unsigned int i = 0; i < 4; i++)
        {
            lightSourceModel = glm::mat4(1.0f);
            lightSourceModel = glm::translate(lightSourceModel, pointLightPositions[i]);
            lightSourceModel = glm::scale(lightSourceModel, glm::vec3(0.2f)); // Make it a smaller cube
            lightSourceShader.setMat4("model", lightSourceModel);
            myCube.Draw(lightSourceShader);
        }

#pragma region DRAW OWN IMPORTS

        importShader.use();
        importShader.setMat4("projection", projection);
        importShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 objModel = glm::mat4(1.0f);
        objModel = glm::translate(objModel, glm::vec3(4.0f, 15.6f, 4.0f)); // translate it down so it's at the center of the scene
        objModel = glm::scale(objModel, glm::vec3(0.3f, 0.3f, 0.3f));	// it's a bit too big for our scene, so scale it down
        objModel = glm::rotate(objModel, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        importShader.setMat4("model", objModel);
        templeModel.Draw(importShader);

        objModel = glm::mat4(1.0f);
        objModel = glm::translate(objModel, glm::vec3(5.0f, 15.6f, 5.0f)); // translate it down so it's at the center of the scene
        objModel = glm::scale(objModel, glm::vec3(0.4f, 0.4f, 0.4f));	// it's a bit too big for our scene, so scale it down
        importShader.setMat4("model", objModel);
        monolithModel.Draw(importShader);

        objModel = glm::mat4(1.0f);
        objModel = glm::translate(objModel, glm::vec3(6.0f, 15.6f, 5.5f)); // translate it down so it's at the center of the scene
        objModel = glm::scale(objModel, glm::vec3(0.4f, 0.4f, 0.4f));	// it's a bit too big for our scene, so scale it down
        importShader.setMat4("model", objModel);
        pyramidModel.Draw(importShader);
#pragma endregion

#pragma region HEIGHTMAP
        heightMapShader.use();

        // view/projection transformations for height map
        glm::mat4 hMapProjection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 hMapView = camera.GetViewMatrix();
        heightMapShader.setMat4("projection", hMapProjection);
        heightMapShader.setMat4("view", hMapView);

        // world transformation
        glm::mat4 hMapModel = glm::mat4(1.0f);
        heightMapShader.setMat4("model", hMapModel);

        /*glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture6);*/
        glBindTexture(GL_TEXTURE_2D, texture7);
        glBindVertexArray(terrainVAO);
        for (unsigned int strip = 0; strip < NUM_STRIPS; ++strip)
        {
            glDrawElements(GL_TRIANGLE_STRIP, NUM_VERTS_PER_STRIP, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int)* NUM_VERTS_PER_STRIP* strip));
        }

#pragma endregion

        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        //glEnable(GL_DEPTH_CLAMP);
        skyboxShader.use();
        glm::mat4 skyModel = glm::mat4(1.0f);
        skyView = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", skyView);
        skyboxShader.setMat4("projection", projection);
        skyboxShader.setMat4("model", skyModel);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

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
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }    
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }  
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        //camSwitch = false;
        if (rightArrowKeyPressed == false)
        {
            //arrowKeyPressed = true;
            cameraIndex++;
            if (cameraIndex > 2)
            {
                cameraIndex = 0;
            }

        }
        
        rightArrowKeyPressed = true;
    }
    else
    {
        rightArrowKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        if (leftArrowKeyPressed == false)
        {
            cameraIndex--;
            if (cameraIndex < 0)
            {
                cameraIndex = 2;
            }
        }
        
        leftArrowKeyPressed = true;
    }
    else
    {
        leftArrowKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        if (spaceKeyPressed == false)
        {
            if (animPlay == false)
            {
                animPlay = true;
            }
            else
            {
                animPlay = false;
            }
        }
        spaceKeyPressed = true;
    }
    else
    {
        spaceKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if (camKeyPressed == false)
        {
            if (camSwitch == false)
            {
                camSwitch = true;
            }
            else
            {
                camSwitch = false;
            }
        }
        camKeyPressed = true;
    }
    else
    {
        camKeyPressed = false;
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

unsigned int loadCubemap(vector<std::filesystem::path> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].generic_string().c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}