#ifndef AVDEMUXER_H
#define AVDEMUXER_H
#include "QThreader.h"
#include <QObject>

#include "AVController.h"
class AVDemuxer : public QThreader
{
    Q_OBJECT
public:
    explicit AVDemuxer(QObject *parent = nullptr);

public:
    bool frameFinished=true;
    AVController*controller;
public slots:
    virtual void start(Priority pri = InheritPriority);
    virtual void stop();
    virtual void pause();
    virtual void resume();
    AVController* initParameters(AVController *controller);
    void freeParameters(AVController *controller);
private:
    virtual void loopRunnable() Q_DECL_OVERRIDE;
};

#endif // AVDEMUXER_H
