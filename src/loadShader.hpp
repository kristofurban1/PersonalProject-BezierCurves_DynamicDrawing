#pragma once


typedef unsigned int OGLID;

OGLID CompileShaderProgram(
    const char* shaderName, 
    const char* vertex_shader, 
    const char* tessCtrl_shader, 
    const char* tessEval_shader, 
    const char* geometry_shader, 
    const char* fragment_shader);