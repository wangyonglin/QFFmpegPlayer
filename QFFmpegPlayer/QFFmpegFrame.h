#ifndef AVFRAMEQUEUE_H
#define AVFRAMEQUEUE_H

#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QThread>

#ifdef __cplusplus
extern "C"
{
#include "libavcodec/avcodec.h"
}
#endif

class AVFrameQueue : public QObject
{
    Q_OBJECT
public:
    explicit AVFrameQueue(QObject *parent = nullptr);

    void enqueue(AVFrame *frame);
    AVFrame* dequeue();
    void clear();
    int size();
    bool isEmpty();
    bool isFulled();

private:
    QQueue<AVFrame*> frameQueue;
    QMutex mutex;
    QWaitCondition waitCondition;
    int this_maxSize=100;
};

#endif // AVFRAMEQUEUE_H
