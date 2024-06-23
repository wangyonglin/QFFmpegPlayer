#ifndef QIMAGEUITLS_H
#define QIMAGEUITLS_H

extern "C" {
#include <libavutil/pixfmt.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include <libavutil/opt.h>
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
#include <libavutil/time.h>
#include <libswresample/swresample.h>
}

#include <QWidget>
#include <QDebug>

class QImageUitls : public QWidget
{
    Q_OBJECT
public:
    explicit QImageUitls(QWidget *parent = nullptr);

    QImage AVFrame2RGBA8888(AVFrame *frame);
signals:
};

#endif // QIMAGEUITLS_H
