#include "Source/Development/Private/DevelZoom.h"

#include "Source/RenderingEngine/Private/RenderingEngine.h"
#include "Source/RenderingEngine/Private/ShaderProgram.h"

#include "Source/GameEngine/Private/Application.h"

DevelZoom::DevelZoom() {
   // Setup the screen vertices
   float screen_vertices[] = {
      -1.0f,  1.0f,  0.0f, 1.0f,
      -1.0f, -1.0f,  0.0f, 0.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
 
      -1.0f,  1.0f,  0.0f, 1.0f,
       1.0f, -1.0f,  1.0f, 0.0f,
       1.0f,  1.0f,  1.0f, 1.0f
   };

   // Setup the screen vao
   glGenVertexArrays(1, &screen_VAO_);
   glGenBuffers(1, &screen_VBO_);
   glBindVertexArray(screen_VAO_);
   glBindBuffer(GL_ARRAY_BUFFER, screen_VBO_);
   glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertices), &screen_vertices, GL_DYNAMIC_DRAW);
   glEnableVertexAttribArray(0);
   glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) 0);
   glEnableVertexAttribArray(1);
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) (2 * sizeof(float)));

   // Enable shader stuff
   RenderingEngine::GetInstance().GetShaderReference("zoom_shader")->Use()->SetInteger("screenTexture", 0);

   // Framebuffer config
   glGenFramebuffers(1, &framebuffer_);
   glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_);

   // Color attachment
   glGenTextures(1, &texture_color_buffer_);
   glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Application::GetInstance().SCREEN_WIDTH, Application::GetInstance().SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
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
}

void DevelZoom::Render(glm::vec3 color) {
   // Bind back to default buffer
   glBindFramebuffer(GL_FRAMEBUFFER, 0);
   glDisable(GL_DEPTH_TEST);

   // Clear relevant buffers
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT);

   // Set the model to be smaller
   glm::mat4 model = glm::mat4(1.0f);
   model = glm::scale(model, glm::vec3(0.5f, 0.5f, 1.0f));

   // Set shader
   ShaderProgram *shader = RenderingEngine::GetInstance().GetShaderReference("texture_shader")->Use();
   shader->SetMatrix4("model", model);
   shader->SetVector3f("color", color);

   // Bind the VAOs
   glBindVertexArray(screen_VAO_);
   glBindTexture(GL_TEXTURE_2D, texture_color_buffer_);
   glDrawArrays(GL_TRIANGLES, 0, 6);
}