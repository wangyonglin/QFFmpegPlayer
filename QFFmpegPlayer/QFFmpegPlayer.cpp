#include "QFFmpegPlayer.h"


QFFmpegPlayer::QFFmpegPlayer(QWidget *parent)
    :QWidget{parent},
    controller(new AVController(this)),
    av_demux(new AVDemuxer(this)),
    audio_dec(new AudioDecoder(this)),
    video_dec(new VideoDecoder(this))
{

    connect(video_dec,&VideoDecoder::drawImage,this,&QFFmpegPlayer::drawImage);
    av_demux->start();
    audio_dec->start();
    video_dec->start();
}

QFFmpegPlayer::~QFFmpegPlayer()
{
    av_demux->stop();
    audio_dec->stop();
    video_dec->stop();
}



void QFFmpegPlayer::play(const QString &url)
{
    controller->url=url;
    av_demux->pause();
    audio_dec->pause();
    video_dec->pause();
    av_demux->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;
    controller->audio_pkt_queue->clear();
    controller->audio_frame_queue->clear();
    controller->video_pkt_queue->clear();
    controller->video_frame_queue->clear();

    if((av_demux->initParameters(controller))){
        controller->audio_stream_index=av_find_best_stream( controller->ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
        controller->video_stream_index=av_find_best_stream( controller->ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
        if( controller->audio_stream_index>=0){
            controller->audio_codecpar= controller->ifmt_ctx->streams[ controller->audio_stream_index]->codecpar;
            controller->audio_pts_base= controller->ifmt_ctx->streams[ controller->audio_stream_index]->time_base;
            controller->audio_pts_begin =  controller->ifmt_ctx->streams[ controller->audio_stream_index]->start_time;
        }
        if( controller->video_stream_index>=0)
        {
            controller->video_codecpar= controller->ifmt_ctx->streams[ controller->video_stream_index]->codecpar;
            controller->video_pts_base= controller->ifmt_ctx->streams[ controller->video_stream_index]->time_base;
            controller->video_pts_begin =  controller->ifmt_ctx->streams[ controller->video_stream_index]->start_time;
        }
    }

    audio_dec->initParameters(controller);
    video_dec->initParameters(controller);
    av_demux->frameFinished=false;
    audio_dec->frameFinished=false;
    video_dec->frameFinished=false;
    controller->init_synchronize();
    av_demux->resume();
    audio_dec->resume();
    video_dec->resume();
}


void QFFmpegPlayer::free()
{
    av_demux->frameFinished=true;
    audio_dec->frameFinished=true;
    video_dec->frameFinished=true;
    av_demux->freeParameters(controller);
    audio_dec->freeParameters(controller);
    video_dec->freeParameters(controller);

}

void QFFmpegPlayer::resizeEvent(QResizeEvent *event)
{
    resize(event->size());
}

void QFFmpegPlayer::paintEvent(QPaintEvent *event)
{
   // beginner
    QPainter painter(this);
    painter.drawImage(0,0,imageUpdate);
}


void QFFmpegPlayer::drawImage(const QImage &image)
{
    imageUpdate = image.scaled(size(),Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
}
