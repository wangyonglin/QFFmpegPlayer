#ifndef QFFMPEG_H
#define QFFMPEG_H

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

#include <QObject>
#include <QDebug>
#include <QImage>

#include "AVPacketQueue.h"
#include "AVFrameQueue.h"


namespace QFFmpeg {

typedef struct{
    AVFormatContext * ifmt_ctx=nullptr;
    char * strurl=nullptr;
    int audio_stream_index=-1;
    int video_stream_index=-1;
    AVCodecParameters * audio_codecpar=nullptr;
    AVCodecParameters * video_codecpar=nullptr;
    AVRational video_pts_base = {0,0};
    AVRational audio_pts_base = {0,0};
    int64_t audio_pts_begin = 0;
    int64_t video_pts_begin = 0;
    bool read_frame_flag =true;
    int64_t audio_frame_dur=0;
    int64_t video_frame_dur=0;
     int64_t expand_size=0;
}QFFmpegDemux;

typedef struct{
    AVCodecContext *codec_ctx=nullptr;
    const AVCodec * codec=nullptr;
    qint64 start_time=0;
    AVRational time_base={0,0};
}QFFmpegDecode;


typedef struct
{
    AVCodecContext *dec_ctx=nullptr;
    struct SwrContext * swr_ctx=nullptr;
    int64_t dst_sample_rate=44100;
    int dst_sample_bytes = 0;
    enum AVSampleFormat dst_sample_fmt=AV_SAMPLE_FMT_S16;
    int dst_nb_channels=0;
    int64_t dst_ch_layout=0;
}QFFmpegResample;
QFFmpegDemux *OpenInitDemux(QString url);
void CloseInitDemux(QFFmpegDemux * ffmpegDemux);
QFFmpegDecode * OpenInitDecode(AVCodecParameters *codecpar);
void CloseInitDecode(QFFmpegDecode * ffmpegDecode);
AVFrame* ReceiveAVFrame(AVCodecContext *codec_ctx, const AVPacket *avpkt);

QFFmpegResample *InitResampler(AVCodecContext *codec_ctx,
                               int64_t dst_sample_rate,
                               AVSampleFormat dst_sample_fmt,
                               int64_t dst_ch_layout);

QByteArray BuildResampler(QFFmpegResample * ffmpegResample,AVFrame *frame);

void FreeResampler(QFFmpegResample * ffmpegResample);



QImage AVFrame2RGBA8888(AVFrame *frame);
AVCodecContext * BuildDecoder(AVCodecParameters *codecpar);
void FreeDecoder(AVCodecContext *codec_ctx);

void ReceiveDecoder(AVCodecContext *codec_ctx,AVPacketQueue *pkt_queue,AVFrameQueue *frame_queue);
void FreeReceiveContext(AVCodecContext *codec_ctx);

AVFrame * ReceiveAVFrame(AVCodecContext *codec_ctx,AVPacketQueue *pkt_queue);
};

#endif // QFFMPEG_H
