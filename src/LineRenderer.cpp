#include "LineRenderer.h"

LineRenderer::LineRenderer(Shader &shader)
{
    this->shader = shader;
}

LineRenderer::~LineRenderer()
{
    glDeleteVertexArrays(1, &axisVAO);
}

void LineRenderer::glBindVertex(unsigned int* VAO)
{
    unsigned int VBO;
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
}

void LineRenderer::glBufData(float* vertex, GLsizeiptr size)
{
    glBufferData(GL_ARRAY_BUFFER, size, vertex, GL_STATIC_DRAW);
}

void LineRenderer::glUnBind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void LineRenderer::PrepareDraw(glm::vec2 position, glm::vec2 size, float rotate)
{
    shader.Use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    shader.SetMatrix4("model", model);
}

void LineRenderer::initAxisData()
{
    float axis[] = { 
        // pos      // color
        -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.0f, -1.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
    };

    glBindVertex(&axisVAO);
    glBufData(axis, sizeof(axis));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2*sizeof(float)));    
    glEnableVertexAttribArray(1);

    glUnBind();
}

void LineRenderer::DrawAxis(glm::vec2 position, glm::vec2 size, float rotate)
{
    PrepareDraw(position, size, rotate);
    glBindVertexArray(axisVAO);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
}

void LineRenderer::initArrowData()
{
    float arrow[] = { 
        // lines      
        0.0f, 0.0f, 1.0f, 0.0f, 
        1.0f, 0.0f, 0.8f, 0.2f,
        1.0f, 0.0f, 0.8f, -0.2f
    };

    glBindVertex(&arrowVAO);
    glBufData(arrow, sizeof(arrow));

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glUnBind();
}

void LineRenderer::DrawArrow(glm::vec2 position, glm::vec2 size, float rotate)
{
    PrepareDraw(position, size, rotate);

    shader.SetVector3f("ourColor", glm::vec3(1.0f, 0.0f, 0.0f));
    glBindVertexArray(arrowVAO);
    glDrawArrays(GL_LINES, 0, 6);
    glBindVertexArray(0);
}
