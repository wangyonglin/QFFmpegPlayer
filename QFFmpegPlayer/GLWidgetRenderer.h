#ifndef GLWIDGETRENDERER_H
#define GLWIDGETRENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QResizeEvent>
#include <QImage>
#include <QDebug>
#include <QOpenGLFunctions>

class GLWidgetRenderer : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit GLWidgetRenderer(QWidget *parent = nullptr);

signals:

    // QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
};

#endif // GLWIDGETRENDERER_H
