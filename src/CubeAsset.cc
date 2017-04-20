#include "CubeAsset.h"

#include <vector>

extern Camera camera;


CubeAsset::CubeAsset(float x, float y, float z, float w, float l, float h, float r, float g, float b) {

  coords = glm::vec3(x, y, z);      // x, y, z world coordinates.
  size   = glm::vec3(w, l, h);      // width, length, height.
  color  = glm::vec4(r, g, b, 1.0); // red, green, blue, alpha values.

  //std::cout << "coords: " << coords[0] << "," << coords[1] << "," << coords[2] << std::endl;

  // Model coordinates, origin at bottom left rear.
  // i.e. the 8 corners of the cube:
  std::vector<GLfloat> vertex_buffer = {
    0, 0, 0, // 0
    0, 1, 0, // 1    5–––––––––7
    1, 0, 0, // 2    | 1–––––3 |
    1, 1, 0, // 3    | |     | |
    0, 0, 1, // 4    | |     | |
    0, 1, 1, // 5    | 0–––––2 |
    1, 0, 1, // 6    4–––––––––6
    1, 1, 1  // 7
  };

  // Realign and resize the cube so the centre is at coordinates "0,0,0":
  for( int i = 0; i < vertex_buffer.size() && i < 99; i++ ) {

    vertex_buffer[i] = vertex_buffer[i] - 0.5;

    switch (i % 3) {
      case 0: vertex_buffer[i] *= size[0]; break;
      case 1: vertex_buffer[i] *= size[1]; break;
      case 2: vertex_buffer[i] *= size[2]; break;
    }

  	//std::cout << "vertex_buffer " << i << " is now: " << vertex_buffer[i] << std::endl;
  }

  // List of coordinates of the triangles that make up the cube, using
  // the coordinates previously specified in the vertex buffer:
  std::vector<GLuint> element_buffer = {
    2, 1, 0, // ⚀ 1
    2, 3, 1,
    6, 3, 2, // ⚁ 2
    6, 7, 3,
    4, 5, 6, // ⚂ 3
    5, 7, 6,
    4, 0, 1, // ⚃ 4
    1, 5, 4,
    6, 0, 4, // ⚄ 5
    6, 2, 0,
    7, 5, 1, // ⚅ 6
    1, 3, 7
  };

  element_buffer_length = element_buffer.size();

  // Transfer buffers to the GPU
  //

  // create buffer
  glGenBuffers(1, &vertex_buffer_token);

  // immediately bind the buffer and transfer the data
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_token);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_buffer.size(), &vertex_buffer.front(), GL_STATIC_DRAW);

  glGenBuffers(1, &element_buffer_token);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_token);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * element_buffer_length, &element_buffer.front(), GL_STATIC_DRAW);
}

CubeAsset::~CubeAsset() {
}

#ifdef DEBUG
#define checkGLError() checkError(__FILE__, __LINE__)
#else
// define symbol to be nothing
#define checkGLError()
#endif

void checkError(std::string file, int line) {
  GLenum gl_error = glGetError();
  if(GL_NO_ERROR != gl_error) {
    std::cerr << "GL error in " << file << " at line " << line << " error: " << gl_error << std::endl;
    exit(-1);
  }
}

void CubeAsset::Draw(GLuint program_token) {
  if(!glIsProgram(program_token)) {
    std::cerr << "Drawing Cube with invalid program" << std::endl;
    return;
  }
  GLint validation_ok;
  glValidateProgram(program_token);
  glGetProgramiv(program_token, GL_VALIDATE_STATUS, &validation_ok);
  if(!validation_ok) {
    GLint maxLength = 0;
    glGetProgramiv(program_token, GL_INFO_LOG_LENGTH, &maxLength);

    //The maxLength includes the NULL character
    std::vector<char> errorLog(maxLength);
    glGetProgramInfoLog(program_token, maxLength, &maxLength, &errorLog[0]);

    std::cerr << "Invalid program " << program_token << " with error code " << validation_ok << std::endl;
    for(auto c: errorLog) {
      std::cerr << c;
    }
    exit(-1);
  }

  GLuint position_attrib = glGetAttribLocation(program_token, "position");
  checkGLError();

  glUseProgram(program_token);
  checkGLError();

  // Grab the location of the coords uniform:
  //GLint uniCoords = glGetUniformLocation(program_token, "cube_coords");
  // Change the value of the uniform to the cube's coordinates:
  //glUniform3f(uniCoords, coords.x, coords.y, coords.z);

  // Grab the location of the fragment color uniform:
  GLint uniColor = glGetUniformLocation(program_token, "cube_color");
  // Change the value of the unifom to the cube's color:
  glUniform4f(uniColor, color[0], color[1], color[2], color[3]);

  glm::mat4 model;
  // Move the cube to it's world space coordinates:
  model = glm::translate(model, coords);
  // Rotate the cube to it's world space rotation (i.e. around it's center point):
  //model = glm::rotate(model, -5.0f, glm::vec3(0.0f, 1.0f, 0.0f));

  glm::mat4 view = camera.GetView();

  glm::mat4 projection;
  projection = glm::perspective(45.0f, 16.0f / 9.0f, 0.1f, 100.0f);

  GLint modelLoc = glGetUniformLocation(program_token, "model");
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
  GLint viewLoc = glGetUniformLocation(program_token, "view");
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  GLint projectionLoc = glGetUniformLocation(program_token, "projection");
  glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // use the previously transferred buffer as the vertex array.  This way
  // we transfer the buffer once -- at construction -- not on every frame.
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_token);
  glVertexAttribPointer(
                        position_attrib,               /* attribute */
                        3,                             /* size */
                        GL_FLOAT,                      /* type */
                        GL_FALSE,                      /* normalized? */
                        0,                             /* stride */
                        (void*)0                       /* array buffer offset */
                        );
  glEnableVertexAttribArray(position_attrib);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_token);
  glDrawElements(
                 GL_TRIANGLES,
                 element_buffer_length,
                 GL_UNSIGNED_INT,
                 (GLvoid*) 0
                 );

  checkGLError();

  glDisableVertexAttribArray(position_attrib);
}
