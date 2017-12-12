#include <mathernogl/MathernoGL.h>

float clamp(float value, float min, float max)
  {
  if (value < min)
    {
    return min;
    }
  if (value > max)
    {
    return max;
    }
  return value;
  }

struct TerrainConfig
  {
  float terrainHeight = 2.63;
  float frequency = 0.068;
  int octaveNum = 5;
  float octaveLac = 2.06;
  float octavePer = 0.343;
  float exp = 4.06;
  float flatToMidStart = 0.005;
  float flatToMidEnd = 0.2;
  float midToSlopeStart = 0.2;
  float midToSlopeEnd = 0.4;
  float heightTexFadeStart = 70;
  float heightTexFadeEnd = 100;
  std::unique_ptr<mathernogl::Texture> flatSurfaceTex;
  std::unique_ptr<mathernogl::Texture> midSurfaceTex;
  std::unique_ptr<mathernogl::Texture> slopeSurfaceTex;
  std::unique_ptr<mathernogl::Texture> heightTex;

  void setInitialVariables(mathernogl::ShaderProgram* shaderProgram, int index)
    {
    std::string prefix = "inTerrainConfig[" + std::to_string(index) + "].";
    shaderProgram->setVarFloat(prefix + "inMaxHeight", terrainHeight);
    shaderProgram->setVarFloat(prefix + "inPersistence", octavePer);
    shaderProgram->setVarFloat(prefix + "inLacunarity", octaveLac);
    shaderProgram->setVarInt(prefix + "inNumOctaves", octaveNum);
    shaderProgram->setVarFloat(prefix + "inFrequency", frequency);
    shaderProgram->setVarFloat(prefix + "inExp", exp);
    shaderProgram->setVarFloat(prefix + "inFlatToMidStart", flatToMidStart);
    shaderProgram->setVarFloat(prefix + "inFlatToMidEnd", flatToMidEnd);
    shaderProgram->setVarFloat(prefix + "inMidToSlopeStart", midToSlopeStart);
    shaderProgram->setVarFloat(prefix + "inMidToSlopeEnd", midToSlopeEnd);
    shaderProgram->setVarFloat(prefix + "inHeightTexFadeStart", heightTexFadeStart);
    shaderProgram->setVarFloat(prefix + "inHeightTexFadeEnd", heightTexFadeEnd);
    shaderProgram->setVarFloat(prefix + "inTextureID", (float)index);

    prefix = "inTextureConfig[" + std::to_string(index) + "].";
    if (flatSurfaceTex)
      {
      glActiveTexture(GL_TEXTURE0 + 0 + index*4);
      glBindTexture(flatSurfaceTex->glTexType, flatSurfaceTex->glTexID);
      shaderProgram->setVarInt(prefix + "inFlatTexture", 0 + index*4);
      }
    if (midSurfaceTex)
      {
      glActiveTexture(GL_TEXTURE0 + 1 + index*4);
      glBindTexture(midSurfaceTex->glTexType, midSurfaceTex->glTexID);
      shaderProgram->setVarInt(prefix + "inMidTexture",  1 + index*4);
      }
    if (slopeSurfaceTex)
      {
      glActiveTexture(GL_TEXTURE0 + 2 + index*4);
      glBindTexture(slopeSurfaceTex->glTexType, slopeSurfaceTex->glTexID);
      shaderProgram->setVarInt(prefix + "inSlopeTexture", 2 + index*4);
      }
    if (heightTex)
      {
      glActiveTexture(GL_TEXTURE0 + 3 + index*4);
      glBindTexture(heightTex->glTexType, heightTex->glTexID);
      shaderProgram->setVarInt("inHeightTexture", 3 + index*4);
      }

    }

  void onInput(mathernogl::InputHandler* inputHandler, mathernogl::ShaderProgram* shaderProgram)
    {
    std::string prefix = "inTerrainConfig[" + std::to_string(0) + "].";
    using namespace mathernogl;
    for (KeyEvent keyEvent : *inputHandler->getKeyEvents())
      {
      if (keyEvent.action == INPUT_HELD)
        {
        switch (keyEvent.key)
          {
          case GLFW_KEY_EQUAL:
            terrainHeight += 0.05;
            shaderProgram->setVarFloat(prefix + "inMaxHeight", terrainHeight);
            break;
          case GLFW_KEY_MINUS:
            terrainHeight -= 0.05;
            shaderProgram->setVarFloat(prefix + "inMaxHeight", terrainHeight);
            break;
          case GLFW_KEY_DOWN:
            octavePer -= 0.001;
            octavePer = clamp(octavePer, 0, 1);
            shaderProgram->setVarFloat(prefix + "inPersistence", octavePer);
            break;
          case GLFW_KEY_UP:
            octavePer += 0.001;
            octavePer = clamp(octavePer, 0, 1);
            shaderProgram->setVarFloat(prefix + "inPersistence", octavePer);
            break;
          case GLFW_KEY_RIGHT:
            octaveLac += 0.001;
            octaveLac = clamp(octaveLac, 1, 999);
            shaderProgram->setVarFloat(prefix + "inLacunarity", octaveLac);
            break;
          case GLFW_KEY_LEFT:
            octaveLac -= 0.001;
            octaveLac = clamp(octaveLac, 1, 999);
            shaderProgram->setVarFloat(prefix + "inLacunarity", octaveLac);
            break;
          case GLFW_KEY_LEFT_BRACKET:
          case GLFW_KEY_RIGHT_BRACKET:
            if (keyEvent.key == GLFW_KEY_LEFT_BRACKET)
              frequency -= 0.0005;
            else
              frequency += 0.0005;
            if (frequency <= 0)
              frequency = 0.001;
            shaderProgram->setVarFloat(prefix + "inFrequency", frequency);
            break;
          case GLFW_KEY_HOME:
          case GLFW_KEY_END:
            if (keyEvent.key == GLFW_KEY_END)
              exp -= 0.02;
            else
              exp += 0.02;
            if (exp < 1)
              exp = 1;
            shaderProgram->setVarFloat(prefix + "inExp", exp);
            break;
          }
        }
      else if (keyEvent.action == INPUT_PRESSED)
        {
        switch (keyEvent.key)
          {
          case GLFW_KEY_L:
            octaveNum++;
            shaderProgram->setVarInt(prefix + "inNumOctaves", octaveNum);
            break;
          case GLFW_KEY_K:
            octaveNum--;
            shaderProgram->setVarInt(prefix + "inNumOctaves", octaveNum);
            break;
          case GLFW_KEY_I:
            logInfo("-----------------------------------");
            logInfo("Height Factor: " + std::to_string(terrainHeight));
            logInfo("Frequency Factor: " + std::to_string(frequency));
            logInfo("Octaves: " + std::to_string(octaveNum));
            logInfo("Lacunarity: " + std::to_string(octaveLac));
            logInfo("Persistance: " + std::to_string(octavePer));
            logInfo("Exp: " + std::to_string(exp));
            logInfo("-----------------------------------");
            break;
          }
        }
      }
    }
  };

int main(int argCount, char* args[])
  {
  using namespace mathernogl;
  try
    {
    int width = 1300, height = 700;
    if (argCount > 2)
      {
      width = atoi(args[1]);
      height = atoi(args[2]);
      }
    Window* window = initGL("TerrainPain", width, height);
//        Window* window = initGL("TerrainPain", 1915, 1025);
    setFaceCulling(true);
    setDepthTest(true);

    ShaderProgram shaderProgram;
    shaderProgram.init({Shader(GL_VERTEX_SHADER, "shaders/TerrainVS.glsl"),
                        Shader(GL_TESS_CONTROL_SHADER, "shaders/TerrainTCS.glsl"),
                        Shader(GL_TESS_EVALUATION_SHADER, "shaders/TerrainTES.glsl"),
                        Shader(GL_GEOMETRY_SHADER, "shaders/TerrainGS.glsl"),
                        Shader(GL_FRAGMENT_SHADER, "shaders/TerrainFS.glsl"),
                       });
    shaderProgram.enable();

    const static int TERRAIN_SIZE = 100;
    const static float SEGMENT_SIZE = 40;
    const static Vector3D SKY_COLOUR(0.2, 0.3, 0.5);

    std::vector<float> vertexData;
    for (int segNumX = 0; segNumX < TERRAIN_SIZE; segNumX++)
      {
      for (int segNumY = 0; segNumY < TERRAIN_SIZE; segNumY++)
        {
        const Vector2D topLeft = Vector2D(segNumX * SEGMENT_SIZE, segNumY * SEGMENT_SIZE);
        const Vector2D topRight = topLeft + Vector2D(SEGMENT_SIZE, 0);
        const Vector2D bottomLeft = topLeft + Vector2D(0, SEGMENT_SIZE);
        const Vector2D bottomRight = topLeft + Vector2D(SEGMENT_SIZE, SEGMENT_SIZE);

        vertexData.push_back(topLeft.x);
        vertexData.push_back(topLeft.y);
        vertexData.push_back(0);
        vertexData.push_back(0);
        vertexData.push_back(bottomLeft.x);
        vertexData.push_back(bottomLeft.y);
        vertexData.push_back(0);
        vertexData.push_back(1);
        vertexData.push_back(bottomRight.x);
        vertexData.push_back(bottomRight.y);
        vertexData.push_back(1);
        vertexData.push_back(1);

        vertexData.push_back(topLeft.x);
        vertexData.push_back(topLeft.y);
        vertexData.push_back(0);
        vertexData.push_back(0);
        vertexData.push_back(bottomRight.x);
        vertexData.push_back(bottomRight.y);
        vertexData.push_back(1);
        vertexData.push_back(1);
        vertexData.push_back(topRight.x);
        vertexData.push_back(topRight.y);
        vertexData.push_back(1);
        vertexData.push_back(0);
        }
      }
    const Vector2D terrainCentre = Vector2D(-(float) TERRAIN_SIZE * SEGMENT_SIZE * 0.5,
                                            -(float) TERRAIN_SIZE * SEGMENT_SIZE * 0.5);
    Vector2D terrainPos = Vector2D(0, 0);
    shaderProgram.setVarVec2("inTerrainPos", terrainPos + terrainCentre);
    shaderProgram.setVarFloat("inFadeStart", TERRAIN_SIZE * SEGMENT_SIZE * 0.5 * 0.7);
    shaderProgram.setVarFloat("inFadeTransition", TERRAIN_SIZE * SEGMENT_SIZE * 0.5 * 0.1);
    shaderProgram.setVarVec3("inFadeColour", SKY_COLOUR);
    window->setClearColour(SKY_COLOUR.x, SKY_COLOUR.y, SKY_COLOUR.z);

    GPUBufferStatic vertexBuffer;
    vertexBuffer.init();
    vertexBuffer.copyDataFloat(vertexData);

    VertexArray vao;
    vao.init();
    vao.bind();
    vao.linkBufferAsFloats(vertexBuffer, 0, 2 * sizeof(float), 2, 0, false);
    vao.linkBufferAsFloats(vertexBuffer, 2 * sizeof(float), 2 * sizeof(float), 2, 1, false);



    Matrix4 camClipTransform = matrixPerspective(1, window->getAspectRatio(), -1, -TERRAIN_SIZE * SEGMENT_SIZE * 0.5);
    shaderProgram.setVarMat4("inCameraToClip", camClipTransform);

    glPatchParameteri(GL_PATCH_VERTICES, 3);
//        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    mathernogl::logInfo("WASD movement, Space/Control for height, Shift for speed.");
    mathernogl::logInfo("M: toggles mouse look.");
    mathernogl::logInfo("P: toggles polygon draw mode.");
    mathernogl::logInfo("K/L: decrease/increase number of octaves.");
    mathernogl::logInfo("-/=: decrease/increase terrain height.");
    mathernogl::logInfo("down/up: decrease/increase persistence.");
    mathernogl::logInfo("left/right: decrease/increase lacunarity.");
    mathernogl::logInfo("[/]: decrease/increase base frequency.");
    mathernogl::logInfo("END/HOME: decrease/increase height exponent.");

    CameraController camera({0, 80, 0}, {0, 0, 0});
    const float CAM_SPEED = 1, SHIFT_MOD = 3.0, ROTATE_SPEED = 0.1;
    bool closeWindow = false, mouseLook = false, polygonFill = true;
    glPolygonMode(GL_FRONT_AND_BACK, polygonFill ? GL_FILL : GL_LINE);
    InputHandler inputHandler;
    inputHandler.init(window->getGLFWWindow());
    inputHandler.setCursorMode(mouseLook ? CURSOR_DISABLED : CURSOR_ENABLED);

    TerrainConfig hilly;
    hilly.flatSurfaceTex.reset(createTextureFromFile("grass.jpg", true));
    hilly.midSurfaceTex.reset(createTextureFromFile("dirt.jpg", true));
    hilly.slopeSurfaceTex.reset(createTextureFromFile("rock.jpg", true));
    hilly.heightTex.reset(createTextureFromFile("snow.jpg", true));
    hilly.setInitialVariables(&shaderProgram, 0);

//    TerrainConfig desert;
//    desert.terrainHeight = 2.13;
//    desert.frequency = 0.0425;
//    desert.octaveNum = 4;
//    desert.octaveLac = 2.08;
//    desert.octavePer = 0.56;
//    desert.exp = 3.1;
//    desert.heightTexFadeStart = 10000;
//    desert.flatToMidStart = 0.002;
//    desert.flatToMidEnd = 0.1;
//    desert.midToSlopeStart = 2;
//    desert.flatSurfaceTex.reset(createTextureFromFile("sand.jpg", true));
//    desert.midSurfaceTex.reset(createTextureFromFile("hard_ground.jpg", true));
//    desert.setInitialVariables(&shaderProgram, 1);

    while (window->isOpen() && !closeWindow)
      {
      inputHandler.checkHeldButtons();

      hilly.onInput(&inputHandler, &shaderProgram);

      if (inputHandler.isKeyAction(GLFW_KEY_ESCAPE, INPUT_PRESSED))
        {
        closeWindow = true;
        }

      Vector3D camTranslation(0);
      float speedMod = 1.0;
      for (KeyEvent keyEvent : *inputHandler.getKeyEvents())
        {
        if (keyEvent.action == INPUT_HELD)
          {
          switch (keyEvent.key)
            {
            case GLFW_KEY_W:
              camTranslation.z -= 1;
              break;
            case GLFW_KEY_S:
              camTranslation.z += 1;
              break;
            case GLFW_KEY_A:
              camTranslation.x -= 1;
              break;
            case GLFW_KEY_D:
              camTranslation.x += 1;
              break;
            case GLFW_KEY_SPACE:
              camTranslation.y += 1;
              break;
            case GLFW_KEY_LEFT_CONTROL:
              camTranslation.y -= 1;
              break;
            case GLFW_KEY_LEFT_SHIFT:
              speedMod = SHIFT_MOD;
              break;
            }
          }
        else if (keyEvent.action == INPUT_PRESSED)
          {
          switch (keyEvent.key)
            {
            case GLFW_KEY_M:
              mouseLook = !mouseLook;
              inputHandler.setCursorMode(mouseLook ? CURSOR_DISABLED : CURSOR_ENABLED);
              break;
            case GLFW_KEY_P:
              polygonFill = !polygonFill;
              glPolygonMode(GL_FRONT_AND_BACK, polygonFill ? GL_FILL : GL_LINE);
              break;
            }
          }
        }


      if (mouseLook)
        {
        Vector2D cursorOffset = inputHandler.getMouseOffset();
        camera.pitch(-cursorOffset.y * ROTATE_SPEED);
        camera.yaw(-cursorOffset.x * ROTATE_SPEED);
        }
      camera.translate( Vector3D(camTranslation.x, 0, camTranslation.z) * camera.getMatrixRotation() * CAM_SPEED * speedMod);
      camera.translate(Vector3D(0, camTranslation.y * CAM_SPEED * speedMod, 0));
      shaderProgram.setVarMat4("inWorldToCamera", matrixTranslate(camera.getPosition() * -1) * matrixInverse(camera.getMatrixRotation()));
      shaderProgram.setVarVec2("inCamPos", Vector2D(camera.getPosition().x, camera.getPosition().z));

#define MOVE_TERRAIN
#ifdef MOVE_TERRAIN
      if (camera.getPosition().x - terrainPos.x > SEGMENT_SIZE / 2)
        {
        terrainPos.x += SEGMENT_SIZE;
        shaderProgram.setVarVec2("inTerrainPos", terrainPos + terrainCentre);
        }
      else if (camera.getPosition().x - terrainPos.x < -SEGMENT_SIZE / 2)
        {
        terrainPos.x -= SEGMENT_SIZE;
        shaderProgram.setVarVec2("inTerrainPos", terrainPos + terrainCentre);
        }
      if (camera.getPosition().z - terrainPos.y > SEGMENT_SIZE / 2)
        {
        terrainPos.y += SEGMENT_SIZE;
        shaderProgram.setVarVec2("inTerrainPos", terrainPos + terrainCentre);
        }
      else if (camera.getPosition().z - terrainPos.y < -SEGMENT_SIZE / 2)
        {
        terrainPos.y -= SEGMENT_SIZE;
        shaderProgram.setVarVec2("inTerrainPos", terrainPos + terrainCentre);
        }
#endif

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glDrawArrays(GL_PATCHES, 0, TERRAIN_SIZE * TERRAIN_SIZE * 6);

      inputHandler.clearEvents();
      window->update();
      }

    delete window;
    }
  catch (const std::runtime_error& e)
    {
    logError(e.what());
    }
  return 0;
  }

