#ifndef DEVELZOOM_H_
#define DEVELZOOM_H_

#include <glm/glm.hpp>

class DevelZoom {
   public:
      // Constructor
      DevelZoom();

      // Set to curr framebuffer
      void SetToFrameBuffer();

      // Now render with overall framebuffer
      void Render(glm::vec3 color = glm::vec3(1.0f));

   private:
      // Screen vars
      unsigned int screen_VAO_;
      unsigned int screen_VBO_;
      unsigned int screen_IBO_;

      // Framebuffer vars
      unsigned int framebuffer_;
      unsigned int texture_color_buffer_;
};

#endif