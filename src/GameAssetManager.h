#ifndef GAMEASSETMANAGER_H
#define GAMEASSETMANAGER_H

#include <memory>
#include <vector>
#include <string>
#include <utility>
#include <fstream>
#include <iostream>

#include <GL/gl.h>

#include "common.h"
#include "Camera.h"
#include "GameAsset.h"

/**
 * GameAssetManager is a container for GameAssets.  It also provides utility
 * functions to to create a simple OpenGL program that can be used to draw a
 * simple GameAsset.
 */
class GameAssetManager {
 public:
  explicit GameAssetManager(ApplicationMode); // Constructor
  virtual ~GameAssetManager();
  GameAssetManager(GameAssetManager const&); // Copy constructor
  GameAssetManager(GameAssetManager const&&); // Move constructor
  void operator=(GameAssetManager const&); // Assignment
  void AddAsset(std::shared_ptr<GameAsset>); // Add asset to end of draw_list
  void AddAsset(std::shared_ptr<GameAsset>, int); // Insert asset into draw_list at specific index
  std::shared_ptr<GameAsset> GetAssetRef(int);
  void Draw();
  void Update(float, float, float);
  void Move(int, float, float, float); // Move the GameAsset at this index int

 private:
  GLuint CreateGLProgram(std::string &, std::string &);
  GLuint CreateGLESShader(GLenum, std::string &);
  // As this is private and we're writing to the GPU, we will use raw pointers.
  std::pair<GLchar *, GLint>  ReadShader(std::string &);

  // The internal scene graph is a simple list.
  std::vector<std::shared_ptr<GameAsset>> draw_list;
  GLuint program_token;
};

#endif // GAMEASSETMANAGER_H
