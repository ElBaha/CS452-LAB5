#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <SOIL/SOIL.h>
#include <chrono>
#include <thread>
#define GLEW_STATIC

using namespace std;

//compiler flags: g++ main.cpp -lGLEW -lGL -lglfw -lSOIL -std=c++11

char* initShaderString(char* filename){
	FILE *fp;
	fp = fopen(filename, "r");//"r" = readonly
	int size;
	char *str;
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	str = (char*)malloc(size + 1);
	fread(str, 1, size, fp);
	str[size] = '\0';
	fclose(fp);
    return str;
}


GLuint initShaders(){
     //shader time!
	char* vertexString = initShaderString("vertexShader.glsl");//dumps contents of vertexShaderl.glsl into vertexString
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);//vertexShader is now designated primary vertex shader
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexString, NULL);//Shader Source is now holding vertex string
    glCompileShader(vertexShader);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char* fragmentString = initShaderString("fragmentShader.glsl");
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentString, NULL);
	glCompileShader(fragmentShader);
    free(vertexString);
	free(fragmentString);

	GLuint shaderProgram = glCreateProgram();//create a shader program
	glAttachShader(shaderProgram, vertexShader);//link vertexShader to it
	glAttachShader(shaderProgram, fragmentShader);//link fragmentShader to it

    glBindFragDataLocation(shaderProgram, 0, "finalColor");//specify which output is writing to which buffer
    glLinkProgram(shaderProgram);//links openGL to our shader program
	glUseProgram(shaderProgram);//designates shaderProgram as currently active shaders

    return shaderProgram;
}

void loadImage(char* filename){
	int width, height;
	unsigned char* image = SOIL_load_image("img.png", &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	SOIL_free_image_data(image);
}

int main(){
	int time = 0;
	glfwInit();
	//std::this_thread::sleep_for(std::chrono::seconds(0));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr); // Windowed
	//GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr) // Fullscreen
    glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE; //force modern OpenGL Method for checking function availability
	glewInit();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    float vertices[] = {
	//  Position      Color             Texcoords
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,// Top-left
        0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // Top-right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right

		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // Bottom-right
		-0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,// Bottom-left
		-0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // Top-left
	};

	GLuint vaoID;//create an ID for a VAO, which stores all the links between attribs and VBOs with raw vertex data
	glGenVertexArrays(1, &vaoID);//make 1 VAO
	glBindVertexArray(vaoID);//start using vao
		//Note: Every time we call glVertexAttribPointer from this point forward, that info will be stored in VAO
        //This means that switching between vertex data/formats is as simple as binding another VAO

	GLuint vboID;//id-to-be for VBO(object that contains all vertex data)
	glGenBuffers(1, &vboID); //Generate 1 buffer(gives vbo a title in OpenGL's context)
	glBindBuffer(GL_ARRAY_BUFFER, vboID);//vboID is now the id of the currently active array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);//put vertices in GL_ARRAY_BUFFER(currently identified by vboID)
		//GL_STATIC_DRAW: Uploaded once, drawn many times(World)
		//GL_DYNAMIC_DRAW: Changed/uploaded occasionally, drawn a lot more
		//GL_STREAM_DRAW: chenge almost every time it's drawn (UI)
		//
	GLuint shaderProgram  = initShaders();


    //Time for Textures!
	GLuint tex;//create ID for texture
	glGenTextures(1, &tex);//tag texture ID
	glBindTexture(GL_TEXTURE_2D, tex);//bind tex to current texture. Tex is a 2D image.
	loadImage("img.png");//load texture from external source
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//setting clamping fo either coord(S,T,R)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//i is for int. fv for float.
    float bordcolor[] = { 1.0f, 0.0f, 0.0f, 1.0f };//red border
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	//set downscale mode to use the two mipmaps that most closely match the size of the pixel being textured and samples with nearest neighbour interpolation
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	//set upscale mode to use the mipmap that most closely matches the size of the pixel being textured and samples with nearest neighbour interpolation
    glGenerateMipmap(GL_TEXTURE_2D);//generate mipmaps from texture


	// Black/white checkerboard
	// float pixels[] = {
	//     0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,
	//         1.0f, 1.0f, 1.0f,   0.0f, 0.0f, 0.0f
	//  };
	//  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_FLOAT, pixels);
	//  //texture type, LOD(0 is base, use for self-mipmap), internal pixel format(on GPU), wdith, height, 5th is always 0, format of pixels in array, array itself
    //  //begins loading at (0,0)

    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");//retrieve ref to position input in vertex shader
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7*(sizeof(GLfloat)), 0); //position is 2 floats long, and only data in VBO. Shove the floats into posAttrib.
		//7*(sizeof(GLfloat)) is pace -- take first 2 floats for every 7 floats in vbo
	glEnableVertexAttribArray(posAttrib);//when you render, actually use this input

    GLint colorAttrib = glGetAttribLocation(shaderProgram, "startColor");//retrieve ref to position input in vertex shader
	glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 7*(sizeof(GLfloat)), (const GLvoid *)(3*(sizeof(GLfloat))));
		//2*(sizeof(GLfloat)) starts at index of 3rd element
	glEnableVertexAttribArray(colorAttrib);//when you render, actually use this input

	//TEXTURE ATTRIBS GO!
	GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 7*(sizeof(GLfloat)), (const GLvoid *)(5*(sizeof(GLfloat))));


	while(!glfwWindowShouldClose(window)){
		glfwSwapBuffers(window);//swap back/front buffers when finished drawing
		glfwPollEvents();//retrieve window events
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		GLfloat strobeColor = glGetUniformLocation(shaderProgram, "triangleStrobe");
		glUniform1f(strobeColor, (sin(time * 3.141592654/180)));
		time++;
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
	}


	glfwTerminate();
}
