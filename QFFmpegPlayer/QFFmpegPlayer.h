#ifndef QFFMPEGPLAYER_H
#define QFFMPEGPLAYER_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLShader>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>

#include "AudioDecoder.h"
#include "AudioRender.h"

#include <QWidget>
#include <QObject>
#include <QString>
#include "AVDemuxer.h"
#include "VideoDecoder.h"

class QFFmpegPlayer : public QOpenGLWidget,public QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:

    explicit QFFmpegPlayer(QWidget *parent = nullptr);
    ~QFFmpegPlayer();

    void play(const QString &url);
    void stop();


private:
    AVDemuxer *av_demux;
    AudioDecoder *audio_dec;
    VideoDecoder *video_dec;
    AVControllerFFmpeg * controller;
    //shader程序
    QOpenGLShaderProgram m_program;
    QOpenGLBuffer vbo;
    int idY,idU,idV;
    int width,height;
    uchar* ptr;
    QImage imageUpdate;
signals:
    void start(QThreader::Priority pri = QThreader::InheritPriority);
    // QOpenGLWidget interface
protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;
};

#endif // QFFMPEGPLAYER_H
