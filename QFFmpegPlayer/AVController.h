#ifndef AVCONTROLLER_H
#define AVCONTROLLER_H

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
#include "AVPacketQueue.h"
#include "AVFrameQueue.h"
#include "AVSynchronize.h"

class AVController : public AVSynchronize
{
    Q_OBJECT
public:
    explicit AVController(QWidget *parent = nullptr);
public:
    AVPacketQueue *audio_pkt_queue=nullptr;
    AVPacketQueue *video_pkt_queue=nullptr;
    AVFrameQueue *audio_frame_queue=nullptr;
    AVFrameQueue *video_frame_queue=nullptr;
    QString url;
    AVFormatContext *ifmt_ctx=nullptr;
    int audio_stream_index=-1;
    int video_stream_index=-1;
    AVCodecParameters * audio_codecpar=nullptr;
    AVCodecParameters * video_codecpar=nullptr;
    AVRational video_pts_base = {0,0};
    AVRational audio_pts_base = {0,0};
    int64_t audio_pts_begin = 0;
    int64_t video_pts_begin = 0;
    AVCodecContext *audio_codec_ctx=nullptr;
    AVCodecContext * video_codec_ctx=nullptr;
    struct SwrContext *swr_ctx;
    int64_t dst_ch_layout =AV_CH_LAYOUT_STEREO;
    int dst_rate=44100;
    enum AVSampleFormat dst_sample_fmt=AV_SAMPLE_FMT_S16;
    uint8_t** src_data_;
    uint8_t** dst_data_;
    int src_nb_channels, dst_nb_channels;
    int src_linesize, dst_linesize;
    int src_nb_samples_, dst_nb_samples_;
    enum AVSampleFormat dst_sample_fmt_;
    enum AVSampleFormat src_sample_fmt_;
    int data_size=0;
private:
    QMutex mutex;
    QWaitCondition waitCondition;
};

#endif // AVCONTROLLER_H
