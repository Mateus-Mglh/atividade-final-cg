#include "graphics/shader.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

static GLuint compileShader(GLenum type, const char* src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetShaderInfoLog(shader, 1024, nullptr, log);
        printf("Shader compile error: %s\n", log);
        std::exit(1);
    }
    return shader;
}

static std::string readFile(const char* path)
{
    std::ifstream in(path, std::ios::in | std::ios::binary);
    if (!in)
    {
        printf("Error opening file: %s\n", path);
        return "";
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

GLuint createShader(const char* vertPath, const char* fragPath)
{
    std::string vsCode = readFile(vertPath);
    std::string fsCode = readFile(fragPath);
    if (vsCode.empty() || fsCode.empty())
    {
        printf("Error: empty shader.\n  VS: %s\n  FS: %s\n", vertPath, fragPath);
        std::exit(1);
    }
    const char* vsSrc = vsCode.c_str();
    const char* fsSrc = fsCode.c_str();
    GLuint vs = compileShader(GL_VERTEX_SHADER,   vsSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    GLint ok = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &ok);
    if (!ok)
    {
        char log[1024];
        glGetProgramInfoLog(program, 1024, nullptr, log);
        printf("Shader link error: %s\n", log);
        std::exit(1);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return program;
}