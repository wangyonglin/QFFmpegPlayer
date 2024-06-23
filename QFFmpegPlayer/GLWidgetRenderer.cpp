#include "GLWidgetRenderer.h"

GLWidgetRenderer::GLWidgetRenderer(QWidget *parent)
    : QOpenGLWidget{parent}
{}
void GLWidgetRenderer::initializeGL()
{
    initializeOpenGLFunctions();

}

void GLWidgetRenderer::resizeGL(int w, int h)
{
    if(h<=0) h=1;

    glViewport(0,0,w,h);
}

void GLWidgetRenderer::paintGL()
{

}
