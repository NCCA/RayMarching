#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include <ngl/Transformation.h>
#include <ngl/NGLInit.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/ShaderLib.h>
#include <QTime>
#include <iostream>

NGLScene::NGLScene()
{
  setTitle("Ray Marching");
}

NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::resizeGL(int _w, int _h)
{
  m_win.width = static_cast<int>(_w * devicePixelRatio());
  m_win.height = static_cast<int>(_h * devicePixelRatio());
}

void NGLScene::initializeGL()
{
  // we must call this first before any other GL commands to load and link the
  // gl commands from the lib, if this is not done program will crash
  ngl::NGLInit::initialize();

  glClearColor(0.4f, 0.4f, 0.4f, 1.0f); // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);
  // we are creating a shader called RayMarch
  ngl::ShaderLib::createShaderProgram("RayMarch");
  // now we are going to create empty shaders for Frag and Vert
  ngl::ShaderLib::attachShader("RayMarchVertex", ngl::ShaderType::VERTEX);
  ngl::ShaderLib::attachShader("RayMarchFragment", ngl::ShaderType::FRAGMENT);
  // attach the source
  ngl::ShaderLib::loadShaderSource("RayMarchVertex", "shaders/RayMarchVertex.glsl");
  ngl::ShaderLib::loadShaderSource("RayMarchFragment", "shaders/RayMarchFragment.glsl");
  // compile the shaders
  ngl::ShaderLib::compileShader("RayMarchVertex");
  ngl::ShaderLib::compileShader("RayMarchFragment");
  // add them to the program
  ngl::ShaderLib::attachShaderToProgram("RayMarch", "RayMarchVertex");
  ngl::ShaderLib::attachShaderToProgram("RayMarch", "RayMarchFragment");
  ngl::ShaderLib::linkProgramObject("RayMarch");
  ngl::ShaderLib::use("RayMarch");
  ngl::ShaderLib::setUniform("mouse", ngl::Vec2(23, 23));
  ngl::ShaderLib::setUniform("cam_pos", ngl::Vec3(0.0, 0.0, 4.0));
  ngl::ShaderLib::setUniform("time", 0.0f);

  m_screenQuad.reset(new ScreenQuad("RayMarch"));
  // as re-size is not explicitly called we need to do this.
  glViewport(0, 0, width(), height());
  startTimer(10);
}

void NGLScene::loadMatricesToShader()
{
  ngl::ShaderLib::setUniform("mouse", ngl::Vec2(m_win.spinXFace, m_win.spinYFace));
  ngl::ShaderLib::setUniform("cam_pos", ngl::Vec3(0.0, 0.0, 4.0));
}

void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  ngl::ShaderLib::use("RayMarch");
  loadMatricesToShader();
  m_screenQuad->draw();
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape:
    QGuiApplication::exit(EXIT_SUCCESS);
    break;
  // turn on wirframe rendering
  case Qt::Key_W:
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    break;
  // turn off wire frame
  case Qt::Key_S:
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  // show full screen
  case Qt::Key_F:
    showFullScreen();
    break;
  // show windowed
  case Qt::Key_N:
    showNormal();
    break;
  default:
    break;
  }
  // finally update the GLWindow and re-draw
  // if (isExposed())
  update();
}

void NGLScene::timerEvent(QTimerEvent *_event)
{
  static float t = 0.0;
  ngl::ShaderLib::use("RayMarch");
  ngl::ShaderLib::setUniform("time", t);
  t += 0.01;
  if (t > 5.0)
    t = 0.0;
  update();
}
