#define STB_IMAGE_IMPLEMENTATION
#include <glad/glad.h>
#include "stb_image.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include <vector>
#include "basicMesh.h"
#include <filesystem>
#include <glm/glm.hpp>
#include "basicCubeMesh.h"

//using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int zc = 0;

int main()
{
#pragma region GLFW_INIT_&_SETUP
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    GLFWwindow* window = glfwCreateWindow(800, 600, "MyGameEngine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);//****** changes

    glViewport(0, 0, 800, 600);

#pragma endregion

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //instantiate shader 
    Shader myShader("resources/shaders/basic.shader.vert", "resources/shaders/basic.shader.frag");

    //load texture
#pragma region TEXTURE

    unsigned int texture1;
    std::filesystem::path imagePath = "resources/textures/cracks.png";

    //texture 1

    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char* data = stbi_load(imagePath.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
#pragma endregion


    //after setup 
    //define model properties


    //convert vertices into basic vertx class
    //std::vector<basicVertex> vertices = {
    //    {glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f,0.0f)},
    //    {glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f,0.0f)},
    //    {glm::vec3(0.0f,  0.5f, 0.0), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f,1.0f)}
    //};

    //basicMesh myTriangle(vertices);

    std::vector<basicCubeVertex> vertices = {
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},

        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},

        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},

        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},

        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},

        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 1.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(1.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 0.0f)},
        {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(1.0f, 2.0f, 3.0f), glm::vec2(0.0f, 1.0f)},
    };
    basicCubeMesh myCube(vertices);



    while (!glfwWindowShouldClose(window))
    {
        //inputs
        processInput(window);

        //processing
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);//BACKGROUND COLOUR
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//******* changes

        //glBindTexture(GL_TEXTURE_2D, texture1);

        //define vector  and initialize it to an identity matrix
        //glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
        //glm::mat4 trans = glm::mat4(1.0);

        //create transformation matrix 
        //trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
        //trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

        //vec = trans * vec;

        //std::cout << vec.x << vec.y << vec.z << std::endl;

        //pass the  transformation matrix to the shader
        //unsigned int transformLoc = glGetUniformLocation(myShader.ID, "transform");

        //get matrix location  and set matrix
        //glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));

        //myShader.setMat4("transform", trans);
       // myTriangle.Draw(myShader);


        //=============================================
        //cube 
        glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

        //THIS ROTATES
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = glm::mat4(1.0f);
        // note that we're translating the scene in the reverse direction of where we want to move
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -20.0f));//THIS SETS THE  CAMERA POSITION

        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        unsigned int viewLoc = glGetUniformLocation(myShader.ID, "view");

        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);



        myShader.setMat4("projection", projection);
        myShader.setMat4("model", model);
        myShader.setVec3("colourIn", glm::vec3(2.0f, 2.0f, 2.0f));

        //CREATE A SECONOD CUBE
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 1.0f, 1.0f));//Coords are X Y Z
        myShader.setMat4("model", model);
        myShader.setVec3("colourIn", glm::vec3(1.0f, 2.0f, 3.0f));

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 4.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, -1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, -3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(1.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);
        //Column 2
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 4.0f, 1.4f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, -1.0f, 1.6f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, -3.0f, 1.3f));
        myShader.setMat4("model", model);
        //Column 3
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, 4.0f, 1.4f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, -1.0f, 1.6f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(3.0f, -3.0f, 1.3f));
        myShader.setMat4("model", model);

        //Column 4
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, 4.0f, 1.4f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, -1.0f, 1.6f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.0f, -3.0f, 1.3f));
        myShader.setMat4("model", model);
        //Column 0
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 4.0f, 1.4f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 1.6f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -3.0f, 1.3f));
        myShader.setMat4("model", model);

        //Column -1
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 4.0f, 1.4f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 1.6f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, -3.0f, 1.3f));
        myShader.setMat4("model", model);

        //Column -2
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 4.0f, 1.4f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -1.0f, 1.6f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -3.0f, 1.3f));
        myShader.setMat4("model", model);

        //Column -3
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 4.0f, 1.4f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 1.6f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, -3.0f, 1.3f));
        myShader.setMat4("model", model);

        //Right Hand Border
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, 4.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, -1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(4.5f, -3.0f, 1.0f));
        myShader.setMat4("model", model);

        //Left Hand Border
        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, 0.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, 1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, 2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, 3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, 4.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, -1.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, -2.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.5f, -3.0f, 1.0f));
        myShader.setMat4("model", model);

        myCube.Draw(myShader);//THIS IS THE CUBE PREFAB





        //math

        //frame buffers

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;

    //return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //all input managing things
}