#include "water.h"

Water::Water() {

}

Water::~Water() {

}

void Water::initialize(const int& width, const int& height) {
    m_Model = glm::mat4(1.0f);
    m_Model = glm::translate(m_Model, { 5.0,0.0,0.0 });

    m_Width = width;
    m_Height = height;

    glGenVertexArrays(1, &m_Vao);
    glBindVertexArray(m_Vao);

    setupIndexMap();
    setupIndices();
    setupVectices();

    glBindVertexArray(0);
}

void Water::draw(Shader* shader, glm::mat4 projection, glm::mat4 view, const float &_time) {

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    shader->use();

    shader->setMat4("model", m_Model);
    shader->setMat4("projection", projection);
    shader->setMat4("view", view);
    shader->setFloat("_amplitude", 1.0f);
    shader->setFloat("_frequency", 2 / 3.0f);
    shader->setFloat("_time", _time);
    shader->setFloat("_speed", 2.0f);

    glBindVertexArray(m_Vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
    glDrawElements(GL_TRIANGLE_STRIP, m_Indices.size(), GL_UNSIGNED_INT, (void*)0);

    glBindVertexArray(0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Water::setupIndexMap() {
    m_IndexMap.clear();
    m_IndexMap.resize(m_Height);
    int idx = 0;
    for (int i = 0; i < m_Height; i++) {
        for (int j = 0; j < m_Width + 2; j++) {
            m_IndexMap[i].push_back(idx);
            if (i == m_Height - 1) {
                if (j == 0 || j == m_Width + 1) {
                    m_IndexMap[i][j] = -1;
                    continue;
                }
            }
            idx++;
        }
    }
}

void Water::setupIndices() {

    glGenBuffers(1, &m_Ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Ebo);
    for (int i = 0; i < m_Height - 1; i++) {
        for (int j = 0; j < m_Width + 2; j++) {
            m_Indices.push_back(m_IndexMap[i][j]);
            if (j > 0 && j < m_Width + 1) {
                m_Indices.push_back(m_IndexMap[i + 1][j]);
            }
        }
    }
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(unsigned int), &m_Indices.at(0), GL_STATIC_DRAW);
}

void Water::setupVectices() {
    for (int i = 0; i < m_Height; i++) {
        for (int j = 0; j < m_Width + 2; j++) {
            if (m_IndexMap[i][j] == -1) continue;
            m_Vertices.push_back(glm::vec3(j - 1, 0.0, i));
            if (j == 0) {
                m_Vertices[m_Vertices.size() - 1].x = j;
                continue;
            }
            if (j == m_Width + 1) {
                m_Vertices[m_Vertices.size() - 1] = glm::vec3(j - 2, 0.0, i + 1);
                continue;
            }
        }
    }
    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    unsigned int sizeBuffer = m_Vertices.size() * 3 * sizeof(float);
    glBufferData(GL_ARRAY_BUFFER, sizeBuffer, &m_Vertices.at(0), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glEnableVertexAttribArray(0);

}