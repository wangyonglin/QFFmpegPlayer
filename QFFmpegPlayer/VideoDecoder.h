#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QObject>
#include "QThreader.h"
#include "AVController.h"

class VideoDecoder : public QThreader
{
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr);
    bool frameFinished= true;
private:
    virtual void loopRunnable();
    AVController * controller;
    void BuildDecoder(AVCodecContext *codec_ctx, AVPacketQueue *pkt_queue, AVFrameQueue *frame_queue);
    QImage AVFrame2RGBA8888(AVFrame *frame);
public slots:
    virtual void start(Priority pri = InheritPriority);
    virtual void stop();
    virtual void pause();
    virtual void resume();
    void freeParameters( AVController * controller);
    AVController *initParameters( AVController * controller);
signals:
    void drawImage(const QImage &image);
};

#endif // VIDEODECODER_H
