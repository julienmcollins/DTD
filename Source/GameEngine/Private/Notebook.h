#ifndef NOTEBOOK_H_
#define NOTEBOOK_H_

class Notebook {
   public:
      // Constructor
      Notebook();

      // Set curr framebuffer
      void SetToFrameBuffer();

      // Render the scene
      void Render();
   private:
      // Buffers
      unsigned int vao_;
      unsigned int vbo_;
      unsigned int ibo_;

      // Attachments
      unsigned int framebuffer_;
      unsigned int texture_color_buffer_;
};

#endif