#ifndef LINE_RENDERER_H
#define LINE_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

class LineRenderer
{
public:
    LineRenderer(Shader& shader);
    ~LineRenderer();
    void initAxisData();
    void initArrowData();
    void DrawAxis(glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f);
    void DrawArrow(glm::vec2 position, glm::vec2 size = glm::vec2(10.0f, 10.0f), float rotate = 0.0f);

private:
    Shader       shader;
    unsigned int axisVAO;
    unsigned int arrowVAO;

    void glBindVertex(unsigned int* VAO);
    void glBufData(float* vertex, GLsizeiptr size);
    void glUnBind();
    void PrepareDraw(glm::vec2 position, glm::vec2 size, float rotate);
};
#endif