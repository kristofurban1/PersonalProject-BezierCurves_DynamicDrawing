#include <iostream>

#include "CurveFitting.hpp"

#include "loadShader.hpp"
#include "Shaders.h"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

GLFWwindow* window;
int width, height;

OGLID connectedLineShader;
OGLID pointShader;
OGLID bezierShader;

glm::mat4 model, view, projection;
void ConstructEnv(){
    width  = 1000;
    height = 1000;
    view = glm::lookAt(
        glm::vec3(0, 0, 5),  // Camera position
        glm::vec3(0, 0, 0),  // Target
        glm::vec3(0, 1, 0)   // Up vector
    );
    model = glm::mat4(1.0f);
}

void Initialize(){
    std::cout << "Initializing..." << std::endl;
    if (!glfwInit()) {
        std::cout << "Initialization failed" << std::endl;
        return;
    }

    glfwWindowHint(GLFW_SAMPLES, 8);
    window = glfwCreateWindow(
        width, height,
        "DynamicDrawing - POC",
        NULL, NULL
    );
    if (window == NULL) {
        std::cout << "Widnow creation failed" << std::endl;
        return;
    }

    glfwMakeContextCurrent(window);

    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        std::cout << "GLAD Load failed!" << std::endl;
        return;
    }

    std::cout << "GLAD Loaded! " << "Version " << GLAD_VERSION_MAJOR(version) << "." << GLAD_VERSION_MINOR(version) << std::endl;
}

void UpdateProjection(){
    float pixelsPerUnit = 100.0f; // Defines how many pixels per world unit
    float halfWidth = (float)width / (2.0f * pixelsPerUnit);
    float halfHeight = (float)height / (2.0f * pixelsPerUnit);

    projection = glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, 0.1f, 100.0f);
}

void WindowSizeChangedCallback(GLFWwindow *window, int _width, int _height){
    width = _width;
    height = _height;
    glViewport(0, 0, width, height);
    UpdateProjection();
}

void CursorWorldPosition(float inX, float inY, float* x, float* y){
    float jankyZ = -0.901902;
    float jankyW = 1;

    float cursorPos_NormClipPosX = inX / width;
    float cursorPos_NormClipPosY = inY / height;

    float cursorPos_NDCx = cursorPos_NormClipPosX * 2 - 1;
    float cursorPos_NDCy = 1 - cursorPos_NormClipPosY * 2;

    glm::vec4 cursorPos_NDC = glm::vec4(cursorPos_NDCx, cursorPos_NDCy, jankyZ, jankyW);
    auto pos = glm::inverse(model * view * projection) * cursorPos_NDC;
    std::cout << "x"<<pos.x<<"y"<<pos.y<< std::endl;
    *x = (float)(pos.x);
    *y = (float)(pos.y);
}

int vertexCount = 0;
int maxVertices = 255;
float verts[255 * 2] = {0};
OGLID pVBO, pVAO;

bool isValid = false;
OGLID bVBO, bVAO;

void RenderBezier(){
    if (vertexCount < 4) return;

    std::vector<Point> points; points.reserve(vertexCount);
    for (size_t i = 0; i < (size_t)vertexCount; i++)
    {
        points.push_back(Point(verts[i*2 + 0], verts[i*2 + 1]));
    }
    
    const Bezier b = FitCubicBezier(points);
    double error = EvaluateBezier(b, points);
    std::cout << "Displaying bezier with error: " << error << std::endl;

    // Start - Control1 - End - Control2 >> P0, P1, P3, P2
    float data[2 * 4] = {b.P0.x, b.P0.y, b.P1.x, b.P1.y, b.P3.x, b.P3.y, b.P2.x, b.P2.y};

    glBindBuffer(GL_ARRAY_BUFFER, bVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 2 * 4, data);

    isValid = true;
}

void WriteVertex(float x, float y){
    if (vertexCount >= maxVertices) return;
    glBindBuffer(GL_ARRAY_BUFFER, pVBO);
    float _vert[2] = {x,y};
    verts[vertexCount*2 + 0] = x;
    verts[vertexCount*2 + 1] = y;
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertexCount, sizeof(float) * 2, _vert); 
    std::cout << "Written vertex " << vertexCount << ": {" << x << "," << y << "}" << std::endl;
    vertexCount++;
}

bool BtnHeld = false;
double prevT = -1;
double delay = 0.05;
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (action == GLFW_REPEAT) return;
    BtnHeld = (action == GLFW_PRESS);
    if (BtnHeld) { vertexCount = 0; isValid = false; }
    if (!BtnHeld) RenderBezier();
    std::cout << "\n" <<std::endl; 
}

void cursor_pos_callback(GLFWwindow *window, double xpos, double ypos){
    if (!BtnHeld) return;

    double t = glfwGetTime();

    if (t - prevT > delay || prevT < -1){
        prevT = t;
        //std::cout << "x"<<xpos<<"y"<<ypos<<" At: " << t << std::endl;
        float x,y;
        CursorWorldPosition(xpos, ypos, &x, &y);
        WriteVertex(x,y);
    }
}

void PrepRender(){
    glGenBuffers(1, &pVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pVBO);

    glGenVertexArrays(1, &pVAO);
    glBindVertexArray(pVAO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 255, verts, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(float) * 2, (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &bVBO); //Generate buffer, retrieve buffer ID
    glBindBuffer(GL_ARRAY_BUFFER, bVBO); //Bind buffer to type, using ID

    glGenVertexArrays(1, &bVAO);
    glBindVertexArray(bVAO);

    float emptyData[2*4] = {0};
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * 4, emptyData, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

}

#ifndef DEBUG_CF
int main(int argc, char const *argv[])
{
    ConstructEnv();
    Initialize();
    pointShader = CompileShaderProgram(LOAD_SHADER_ControlPointShader);
    connectedLineShader = CompileShaderProgram(LOAD_SHADER_ConnectedLineShader);
    bezierShader = CompileShaderProgram(LOAD_SHADER_BezierShader);

    PrepRender();

    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetWindowSizeCallback(window, WindowSizeChangedCallback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    
    UpdateProjection();
    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT);

        if(vertexCount > 0){
            glUseProgram(pointShader);
            glUniform2f(glGetUniformLocation(pointShader, "uResolution"), width, height);
            
            GLuint modelLoc = glGetUniformLocation(pointShader, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            GLuint viewLoc = glGetUniformLocation(pointShader, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            
            GLuint projectionLoc = glGetUniformLocation(pointShader, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            GLuint thicknessLoc = glGetUniformLocation(pointShader, "size");
            glUniform1f(thicknessLoc, 15);
            
            glBindVertexArray(pVAO);
            glDrawArrays(GL_POINTS, 0, vertexCount);
        }    

        if(vertexCount > 1){
            glUseProgram(connectedLineShader);
            glUniform2f(glGetUniformLocation(connectedLineShader, "uResolution"), width, height);
            
            GLuint modelLoc = glGetUniformLocation(connectedLineShader, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            
            GLuint viewLoc = glGetUniformLocation(connectedLineShader, "view");
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
            
            GLuint projectionLoc = glGetUniformLocation(connectedLineShader, "projection");
            glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

            GLuint thicknessLoc = glGetUniformLocation(connectedLineShader, "thickness");
            glUniform1f(thicknessLoc, 8);
            
            glBindVertexArray(pVAO);
            glDrawArrays(GL_LINE_STRIP, 0, vertexCount);
        }


        glUseProgram(bezierShader);
        glUniform2f(glGetUniformLocation(bezierShader, "uResolution"), width, height);
        
        GLuint modelLoc = glGetUniformLocation(bezierShader, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        GLuint viewLoc = glGetUniformLocation(bezierShader, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        
        GLuint projectionLoc = glGetUniformLocation(bezierShader, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        GLuint thicknessLoc = glGetUniformLocation(bezierShader, "thickness");
        glUniform1f(thicknessLoc, 8);

        GLuint isValidLoc = glGetUniformLocation(bezierShader, "isValid");
        glUniform1d(isValidLoc, isValid);
        
        glBindVertexArray(bVAO);
        glPatchParameteri(GL_PATCH_VERTICES, 2);
        glDrawArrays(GL_PATCHES, 0, 2);
    
        // swap buffers and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0; 
}

#endif