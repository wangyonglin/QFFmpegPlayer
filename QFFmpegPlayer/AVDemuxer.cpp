#include "AVDemuxer.h"
AVDemuxer::AVDemuxer(QObject *parent)
    : QThreader{parent}
{

}
AVController* AVDemuxer::initParameters(AVController *controller){
    if(controller->url.isEmpty())return nullptr;
    if(controller->ifmt_ctx)avformat_free_context( controller->ifmt_ctx);
    controller->ifmt_ctx=nullptr;
    int read_ret = avformat_open_input(&controller->ifmt_ctx, controller->url.toLocal8Bit().data(), NULL, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avformat_open_input failed" << errmsg;
        return nullptr;
    }
    read_ret = avformat_find_stream_info(controller->ifmt_ctx, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avformat_find_stream_info failed" << errmsg;
        avformat_close_input(&controller->ifmt_ctx);
        return nullptr;
    }
    av_dump_format(controller->ifmt_ctx, 0, controller->url.toLocal8Bit().data(), 0);
    this->controller=controller;
    return controller;

}
void AVDemuxer::freeParameters(AVController *controller){
    if(controller->ifmt_ctx){
        avformat_close_input(&controller->ifmt_ctx);
        controller->ifmt_ctx=nullptr;
    }
}

void AVDemuxer::start(Priority pri)
{
    QThreader::start(pri);
}

void AVDemuxer::stop()
{

    QThreader::stop();
}

void AVDemuxer::pause()
{

    QThreader::pause();

}

void AVDemuxer::resume()
{
    QThreader::resume();
}

void AVDemuxer::loopRunnable()
{
    if(frameFinished){
        QThreader::usleep(10);
    }
    if(state() ==Running && !frameFinished){

        controller->start_synchronize();
        if(controller->audio_pkt_queue->size() > 10 || controller->video_pkt_queue->size() >10){
            QThread::usleep(200);
            return;
        }
        AVPacket* pkt=av_packet_alloc();
        av_packet_unref(pkt);
        int read_ret = av_read_frame(controller->ifmt_ctx, pkt);
        if (read_ret < 0) {
            char errmsg[AV_ERROR_MAX_STRING_SIZE];
            av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
            if (read_ret == AVERROR_EOF)
            {
                qDebug() << "Reached end of file" << errmsg;
                av_packet_unref(pkt);
                frameFinished=true;
                return;
            }
            else
            {
                qDebug() << "Error while reading frames" << errmsg;
                return;
            }
        }
        if (pkt->stream_index == controller->audio_stream_index)
        {
            controller->audio_pkt_queue->enqueue(pkt);
        }
        else if(pkt->stream_index == controller->video_stream_index)
        {
            controller->video_pkt_queue->enqueue(pkt);
        }
        av_packet_free(&pkt);
    }

}
