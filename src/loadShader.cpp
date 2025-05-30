
#include "loadShader.hpp"

#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "errorhandler.h"


OGLID CompileShader(const char* shaderName, const char* shader, GLenum type, const char* debugInfo){
    if (shader == NULL) return 0;
    OGLID shaderID;
    Info("Loading %s %s shader!\n", shaderName, debugInfo);
    shaderID = glCreateShader(type);
    glShaderSource(shaderID, 1, &shader, NULL);
    glCompileShader(shaderID);

    int success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(shaderID, 510, NULL, infoLog);
        Error(infoLog);
        PANIC(1, "CompileShader - ERROR:SHADER:COMPILATION_FAILED");
    }
    return shaderID;
}

void AttachShader(OGLID program, OGLID shaderID, const char* shader){
    if (shader != NULL) 
        glAttachShader(program, shaderID);
}

void DeleteShader(OGLID shaderID, const char* shader){
    if (shader != NULL) 
        glDeleteShader(shaderID);
}

OGLID CompileShaderProgram(
    const char* shaderName, 
    const char* vertex_shader, 
    const char* tessCtrl_shader, 
    const char* tessEval_shader, 
    const char* geometry_shader, 
    const char* fragment_shader){
    // Load and compile vertexShader

    Info("Compliling shader program (%s)...", shaderName);

    OGLID vertex    = CompileShader(shaderName, vertex_shader, GL_VERTEX_SHADER, "vertex");
    OGLID tcs       = CompileShader(shaderName, tessCtrl_shader, GL_TESS_CONTROL_SHADER, "tessellation control");
    OGLID tes       = CompileShader(shaderName, tessEval_shader, GL_TESS_EVALUATION_SHADER, "tessellation evalution");
    OGLID geometry  = CompileShader(shaderName, geometry_shader, GL_GEOMETRY_SHADER, "geometry");
    OGLID fragment  = CompileShader(shaderName, fragment_shader, GL_FRAGMENT_SHADER, "fragment");


    // Creating and shaderprogram and linking it together
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    
    AttachShader(shaderProgram, vertex, vertex_shader);
    AttachShader(shaderProgram, tcs, tessCtrl_shader);
    AttachShader(shaderProgram, tes, tessEval_shader);
    AttachShader(shaderProgram, geometry, geometry_shader);
    AttachShader(shaderProgram, fragment, fragment_shader);

    glLinkProgram(shaderProgram);

    
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        Error(infoLog);
        PANIC(1, "CompileShaderProgram - ERROR:SHADER:PROGRAM:LINKING_FAILED");
    }
    
    // shaders are integrated into the program, thus can be discarded
    
    DeleteShader(vertex, vertex_shader);
    DeleteShader(tcs, tessCtrl_shader);
    DeleteShader(tes, tessEval_shader);
    DeleteShader(geometry, geometry_shader);
    DeleteShader(fragment, fragment_shader);

    Info("Compliation Successful!");
    
    return shaderProgram;
}