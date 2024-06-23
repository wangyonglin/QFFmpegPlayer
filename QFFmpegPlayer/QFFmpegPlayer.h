#ifndef QFFMPEGPLAYER_H
#define QFFMPEGPLAYER_H


#include "AudioDecoder.h"
#include "AudioRender.h"

#include "VideoRender.h"
#include <QWidget>
#include <QObject>
#include <QString>
#include "AVDemuxer.h"
#include "VideoDecoder.h"

class QFFmpegPlayer : public QWidget
{
    Q_OBJECT
public:

    explicit QFFmpegPlayer(QWidget *parent = nullptr);
    ~QFFmpegPlayer();

    void play(const QString &url);
    void free();

public slots:
    void drawImage(const QImage &image);
private:
    AVDemuxer *av_demux;
    AudioDecoder *audio_dec;
    VideoDecoder *video_dec;
    AVController * controller;
    QImage imageUpdate;
signals:
    void start(QThreader::Priority pri = QThreader::InheritPriority);
protected:
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
};

#endif // QFFMPEGPLAYER_H
