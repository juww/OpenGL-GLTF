#pragma once
#ifndef GRASS_H
#define GRASS_H

#include <vector>
#include <glm/glm.hpp>

#include "shader_m.h"

const int N_VERTEX = 45;

class Grass {
public:
	const float vectices[N_VERTEX] = {
		0.012f, 0.000f, 0.000f,
		0.088f, 0.000f, 0.000f,
		0.013f, 0.100f, 0.000f,
		0.087f, 0.100f, 0.000f,
		0.016f, 0.187f, 0.000f,
		0.084f, 0.187f, 0.000f,
		0.021f, 0.264f, 0.000f,
		0.079f, 0.264f, 0.000f,
		0.025f, 0.336f, 0.000f,
		0.075f, 0.336f, 0.000f,
		0.031f, 0.394f, 0.000f,
		0.068f, 0.394f, 0.000f,
		0.038f, 0.440f, 0.000f,
		0.061f, 0.440f, 0.000f,
		0.050f, 0.485f, 0.000f,
	};
	glm::vec3 pos, rot, scale;
	glm::mat4 model;
	unsigned int vao, ebo;
	int width, height;
	int density;
	int count;

	void initialize(const int &w, const int &h, const int &d) {
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-0.050f, 0.0f, 0.0f));

		width = w;
		height = h;
		density = d;

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		std::vector<unsigned int> indices;
		for (int i = 0; i < (N_VERTEX / 3); i++) {
			indices.push_back(i);
		}
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices.at(0), GL_STATIC_DRAW);

		unsigned int vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, N_VERTEX * sizeof(float), &vectices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void setPositionGrass(const std::vector<glm::vec3>& posOffset) {
		glBindVertexArray(vao);

		unsigned int instanceVbo;
		glGenBuffers(1, &instanceVbo);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVbo);

		glBufferData(GL_ARRAY_BUFFER, posOffset.size() * sizeof(float) * 3, &posOffset.at(0), GL_STATIC_DRAW);
		count = posOffset.size();
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
		glEnableVertexAttribArray(1);

		glVertexAttribDivisor(1, 1);

		printf("count Grass = %d\n", count);
		glBindVertexArray(0);
	}

	void update() {

	}

	void draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {

		shader.use();
		
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setFloat("halfOffset", float(width) / 2.0f + 0.5f);

		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glDrawElementsInstanced(GL_TRIANGLE_STRIP, (N_VERTEX / 3) * sizeof(unsigned int), GL_UNSIGNED_INT, (void*)0, count);

		glBindVertexArray(0);
	}
};

#endif