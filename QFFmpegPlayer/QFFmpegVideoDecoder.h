#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QObject>
#include "QThreader.h"
#include "AVControllerFFmpeg.h"

class VideoDecoder : public QThreader
{
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr);
    bool frameFinished= true;
private:
    virtual void loopRunnable();
    AVControllerFFmpeg * controller;
    AVFrame         *yuvFrame     = NULL;
    void BuildDecoder(AVCodecContext *codec_ctx, AVPacketQueue *pkt_queue, AVFrameQueue *frame_queue);
public slots:
    virtual void start(Priority pri = InheritPriority);
    virtual void stop();
    virtual void pause();
    virtual void resume();
    void freeParameters( AVControllerFFmpeg * controller);
    AVControllerFFmpeg *initParameters( AVControllerFFmpeg * controller);
signals:
    void drawImage(const QImage &image);
    void sigFirst(uchar* p,int w,int h);
    void newFrame();
};

#endif // VIDEODECODER_H
