#include "Source/GameEngine/Private/Notebook.h"
#include "Source/RenderingEngine/Private/RenderingEngine.h"
#include "Source/RenderingEngine/Private/ShaderProgram.h"
#include "Source/GameEngine/Private/Application.h"

Notebook::Notebook() {
   // Setup the screen vertices
   float vertices[] = {
      // positions  // texture coords
      1.0,  1.0,    1.0, 1.0, // top right
      1.0, -1.0,    1.0, 0.0, // bottom right
     -1.0, -1.0,    0.0, 0.0, // bottom left
     -1.0,  1.0,    0.0, 1.0  // top left
   };

   // Indices
   unsigned int indices[] = {
      3, 1, 2, // First triangle
      3, 0, 1  // Second triangle
   };

   // Setup the screen vao
   glGenVertexArrays(1, &vao_);
   glGenBuffers(1, &vbo_);
   glGenBuffers(1, &ibo_);

   // Bind vao
   glBindVertexArray(vao_);

   // Bind vbo
   glBindBuffer(GL_ARRAY_BUFFER, vbo_);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_DYNAMIC_DRAW);
   
   // Bind ibo
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_DYNAMIC_DRAW);

   // Set appropriate pointers
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

   // Framebuffer config
   glGenFramebuffers(1, &framebuffer_);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

   // Color attachment
   glGenTextures(1, &texture_color_buffer_);
   glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1920.0f, 1080.0f, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_color_buffer_, 0);

   // Don't create rbo for now

   // Check for everything good
   if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      std::cout << "ERROR: FRAMEBUFFER is not complete\n";
   }
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Notebook::SetToFrameBuffer() {
   // Bind frame buffer
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
   glEnable(GL_DEPTH_TEST);
   // glClearColor(0.8f, 0.79f, 0.78f, 1.0f);
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Notebook::Render() {
      // Bind back to default buffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDisable(GL_DEPTH_TEST);

   // Clear relevant buffers
   // glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
   // glClear(GL_COLOR_BUFFER_BIT);

   // Set the model to be smaller
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::scale(model, glm::vec3(0.9f, 0.9f, 1.0f));

   // Set shader
   ShaderProgram *shader = RenderingEngine::GetInstance().GetShaderReference("zoom_shader")->Use();
   shader->SetMatrix4("model", model);

   // Bind the VAOs
   glBindVertexArray(vao_);
   glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}