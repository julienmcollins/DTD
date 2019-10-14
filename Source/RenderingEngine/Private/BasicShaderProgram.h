#ifndef BASICSHADERPROGRAM_H_
#define BASICSHADERPROGRAM_H_

#include "ShaderProgram.h"
#include "GLData.h"

class BasicShaderProgram : public ShaderProgram {
    public:
        // Get instance
        static BasicShaderProgram& get_instance() {
            static BasicShaderProgram instance;
            return instance;
        }

        // Load program
        virtual bool load_program();

        // Set the vertex pointer, attribute
        void set_vertex_pointer(GLsizei stride, const GLvoid *data);

        // Set the color attribute pointer
        void set_tex_pointer(GLsizei stride, const GLvoid *data);

        // Enable all the data pointers
        void enable_data_pointers();

        // Disable all the data pointers
        void disable_data_pointers();

        // Set the projection matrix
        void set_projection_matrix(glm::mat4 matrix);

        // Set modelview matrix
        void set_modelview_matrix(glm::mat4 matrix);

        // Left multiply projection
        void left_mult_projection(glm::mat4 matrix);

        // Let multiple modelview
        void left_mult_modelview(glm::mat4 matrix);

        // Update the projection matrix
        void update_projection_matrix();

        // Update the modelview matrix
        void update_modelview_matrix();

        // Set the texture color
        void set_texture_color(GLColorRGBA color);

        // Set the texture unit
        void set_texture_unit(unsigned int unit);

    private:
        // Construct basic shader program
        BasicShaderProgram();

        // Attribute locations
        int vertex_pos2D_location_;
        int tex_coord_location_;
        int color_location_;

        // Texture unit location
        int tex_unit_location_;

        // Projection matrix
        glm::mat4 projection_matrix_;
        int projection_matrix_location_;

        // Model view matrix
        glm::mat4 modelview_matrix_;
        int modelview_matrix_location_;

    public:
      // More singleton stuff
      BasicShaderProgram(BasicShaderProgram const&) = delete;
      void operator=(BasicShaderProgram const&) = delete;
};

#endif