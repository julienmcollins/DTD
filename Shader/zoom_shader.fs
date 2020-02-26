#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
   vec4 col = texture(screenTexture, TexCoords);
   if (col.a == 0.0) {
      FragColor = vec4(1.0, 1.0, 1.0, 1.0);
   } else {
      FragColor = vec4(col.rgb, 1.0);
   }
}