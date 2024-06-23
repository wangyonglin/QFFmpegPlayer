#ifndef AUDIODECODER_H
#define AUDIODECODER_H
#include <QPointer>
#include "QFFmpeg.h"
#include "QThreader.h"
#include "AudioRender.h"
#include "AVPacketQueue.h"
#include "AVFrameQueue.h"
#include "AVResample.h"
#include "AVControllerFFmpeg.h"
class AudioDecoder : public QThreader
{
    Q_OBJECT
public:
    explicit AudioDecoder(QObject *parent = nullptr);
    ~AudioDecoder();

   bool frameFinished= true;
private:
    virtual void loopRunnable();
    AVResample av_resample;
    AudioRender audio_render;
    AVControllerFFmpeg * controller;
    void BuildDecoder(AVCodecContext *codec_ctx, AVPacketQueue *pkt_queue, AVFrameQueue *frame_queue);
public slots:
    virtual void start(Priority pri = InheritPriority);
    virtual void stop();
    virtual void pause();
    virtual void resume();
    void freeParameters(AVControllerFFmpeg * controller);
    AVControllerFFmpeg *initParameters(AVControllerFFmpeg * controller);
};
#endif // AUDIODECODER_H
