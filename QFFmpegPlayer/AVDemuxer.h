#ifndef AVDEMUXER_H
#define AVDEMUXER_H
#include "QThreader.h"
#include <QObject>

#include "AVControllerFFmpeg.h"
class AVDemuxer : public QThreader
{
    Q_OBJECT
public:
    explicit AVDemuxer(QObject *parent = nullptr);

public:
    bool frameFinished=true;
    AVControllerFFmpeg*controller;
public slots:
    virtual void start(Priority pri = InheritPriority);
    virtual void stop();
    virtual void pause();
    virtual void resume();
    AVControllerFFmpeg* initParameters(AVControllerFFmpeg *controller);
    void freeParameters(AVControllerFFmpeg *controller);
private:
    virtual void loopRunnable() Q_DECL_OVERRIDE;
};

#endif // AVDEMUXER_H
