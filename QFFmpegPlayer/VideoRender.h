#ifndef VIDEORENDER_H
#define VIDEORENDER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QResizeEvent>
#include <QImage>
#include <QDebug>
#include <QOpenGLFunctions>

class VideoRender : public QOpenGLWidget,QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    explicit VideoRender(QWidget *parent = nullptr);
    ~VideoRender();

protected:
    virtual void initializeGL() override;
    virtual void paintGL() override;
    virtual void resizeEvent(QResizeEvent *e) override;
    virtual void resizeGL(int w, int h) override;  // 这个没有重写的必要
private:
    void initShaders();
    void initTextures();
private:
    QVector<QVector3D> vertices;
    QVector<QVector2D> texCoords;
    QOpenGLShaderProgram program;
    QOpenGLTexture *texture;
    QMatrix4x4 projection;
    QImage image;
public slots:
    void loadImage(const QImage &image);
};

#endif // VIDEORENDER_H
