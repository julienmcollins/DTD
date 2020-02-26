#include "Source/GameEngine/Development/Private/DevelZoom.h"

#include "Source/GameEngine/RenderingEngine/Private/RenderingEngine.h"
#include "Source/GameEngine/RenderingEngine/Private/ShaderProgram.h"

#include "Source/GameEngine/Engine/Private/Application.h"

DevelZoom::DevelZoom() {
   // Setup the screen vertices
   float screen_vertices[] = {
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
   glGenVertexArrays(1, &screen_VAO_);
   glGenBuffers(1, &screen_VBO_);
   glGenBuffers(1, &screen_IBO_);

   // Bind vao
   glBindVertexArray(screen_VAO_);

   // Bind vbo
   glBindBuffer(GL_ARRAY_BUFFER, screen_VBO_);
   glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), &screen_vertices, GL_DYNAMIC_DRAW);
   
   // Bind ibo
   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, screen_IBO_);
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

void DevelZoom::SetToFrameBuffer() {
   // Bind frame buffer
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);
   glEnable(GL_DEPTH_TEST);
   glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void DevelZoom::Render(glm::vec3 color) {
   // Bind back to default buffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDisable(GL_DEPTH_TEST);

   // Clear relevant buffers
   // glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
   // glClear(GL_COLOR_BUFFER_BIT);

   // Set the model to be smaller
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::translate(model, glm::vec3(0.5f, 0.5f, 0.0f));
   model = glm::scale(model, glm::vec3(0.33f, 0.33f, 1.0f));

   // Set shader
   ShaderProgram *shader = RenderingEngine::GetInstance().GetShaderReference("zoom_shader")->Use();
   shader->SetMatrix4("model", model);

   // Get mouse state and buffer data based on this
   int x, y;
   SDL_GetMouseState(&x, &y);

   // Normalize coordinates
   float normal_x = (float) (x / 1920.0f), normal_y = (float) (-y / 1080.0f);

   // Create vertices based on mouse state
   float screen_vertices[] = {
      // positions  // texture coords
      1.0f,  1.0f,  normal_x + 0.1f, normal_y + 0.1f, // top right
      1.0f, -1.0f,  normal_x + 0.1f, normal_y - 0.1f, // bottom right
     -1.0f, -1.0f,  normal_x - 0.1f, normal_y - 0.1f, // bottom left
     -1.0f,  1.0f,  normal_x - 0.1f, normal_y + 0.1f  // top left
   };

   // Bind the VAOs
   glBindVertexArray(screen_VAO_);
   glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
   glBindBuffer(GL_ARRAY_BUFFER, screen_VBO_);
   glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(screen_vertices), screen_vertices);
   glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}