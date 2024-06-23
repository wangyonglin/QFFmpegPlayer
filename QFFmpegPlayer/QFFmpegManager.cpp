#include "AVControllerFFmpeg.h"

AVControllerFFmpeg::AVControllerFFmpeg(QWidget *parent)
    : AVSynchronize{parent},

    audio_pkt_queue(new AVPacketQueue(this)),
    video_pkt_queue(new AVPacketQueue(this)),
    audio_frame_queue(new AVFrameQueue(this)),
    video_frame_queue(new AVFrameQueue(this))
{}
