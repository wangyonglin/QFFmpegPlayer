#include "AVController.h"

AVController::AVController(QWidget *parent)
    : AVSynchronize{parent},

    audio_pkt_queue(new AVPacketQueue(this)),
    video_pkt_queue(new AVPacketQueue(this)),
    audio_frame_queue(new AVFrameQueue(this)),
    video_frame_queue(new AVFrameQueue(this))
{}
