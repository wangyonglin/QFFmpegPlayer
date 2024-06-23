#include "VideoDecoder.h"

VideoDecoder::VideoDecoder(QObject *parent)
    : QThreader{parent}
{}

AVControllerFFmpeg *VideoDecoder::initParameters( AVControllerFFmpeg * controller){
    controller->video_codec_ctx = avcodec_alloc_context3(NULL);
    if(!  controller->video_codec_ctx)return nullptr;
    int read_ret = avcodec_parameters_to_context(  controller->video_codec_ctx, controller->video_codecpar);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_parameters_to_context failed" << errmsg;
        avcodec_free_context(&  controller->video_codec_ctx);
        return nullptr;
    }
    // h264
    // h264_qsv  AV_CODEC_ID_H264
    //    avcodec_find_decoder_by_name()

    //    if(AV_CODEC_ID_H264 == codec_ctx_->codec_id)
    //        codec = avcodec_find_decoder_by_name("h264_qsv");
    //    else
const    AVCodec * codec = avcodec_find_decoder(  controller->video_codec_ctx->codec_id); //作业： 硬件解码
    if(!codec) {
        qDebug() << "avcodec_find_decoder failed";
        avcodec_free_context(&  controller->video_codec_ctx);
        return nullptr;
    }

    read_ret = avcodec_open2(  controller->video_codec_ctx, codec, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_open2 failed" << errmsg;
        avcodec_free_context(&  controller->video_codec_ctx);
        return nullptr;
    }
    controller->YUV420BufferSize = av_image_get_buffer_size(
        AV_PIX_FMT_YUV420P,
        controller->video_codec_ctx->width,
        controller->video_codec_ctx->height,1);
    controller->YUV420Buffer = (unsigned char *)av_malloc(controller->YUV420BufferSize*sizeof(uchar));
    emit sigFirst( controller->YUV420Buffer,controller->video_codec_ctx->width,controller->video_codec_ctx->height);
    this->controller=controller;
    return  controller;
}

void VideoDecoder::freeParameters(AVControllerFFmpeg * controller){
    if( controller->video_codec_ctx){
        avcodec_free_context(& controller->video_codec_ctx);
        av_freep(controller->YUV420Buffer);
        controller->YUV420Buffer=nullptr;
        controller->video_codec_ctx=nullptr;
    }
}

void VideoDecoder::loopRunnable()
{
    if(frameFinished){
        QThreader::usleep(10);
    }
    if(state()==Running && !frameFinished){
        if (controller->get_video_synchronize() > controller->get_master_synchronize())
        {
             QThread::usleep(10);
            return;
        }

        if(!controller->video_pkt_queue->isEmpty()){
            if(controller->video_frame_queue->size()  <10){
                BuildDecoder(controller->video_codec_ctx,controller->video_pkt_queue,controller->video_frame_queue);
            }else{
                QThread::usleep(200);
                return;
            }
        }

        if(!controller->video_frame_queue->isEmpty()){
            AVFrame * yuvFrame= controller->video_frame_queue->dequeue();
            if(yuvFrame){
               int64_t pts_time= (yuvFrame->pts == AV_NOPTS_VALUE) ? NAN : yuvFrame->pts;
                controller->video_synchronize(pts_time,controller->video_pts_begin,controller->video_pts_base);
                int bytes =0;
                for(int i=0;i<controller->video_codec_ctx->height;i++){
                    memcpy( controller->YUV420Buffer+bytes,yuvFrame->data[0]+yuvFrame->linesize[0]*i,controller->video_codec_ctx->width);
                    bytes+=controller->video_codec_ctx->width;
                }

                int u=controller->video_codec_ctx->height>>1;
                for(int i=0;i<u;i++){
                    memcpy( controller->YUV420Buffer+bytes,yuvFrame->data[1]+yuvFrame->linesize[1]*i,controller->video_codec_ctx->width/2);
                    bytes+=controller->video_codec_ctx->width/2;
                }

                for(int i=0;i<u;i++){
                    memcpy( controller->YUV420Buffer+bytes,yuvFrame->data[2]+yuvFrame->linesize[2]*i,controller->video_codec_ctx->width/2);
                    bytes+=controller->video_codec_ctx->width/2;
                }

                emit newFrame();
                av_frame_free(&yuvFrame);
            }

        }
    }
}

void VideoDecoder::start(Priority pri)
{
    QThreader::start(pri);
}

void VideoDecoder::stop()
{
    QThreader::stop();
}

void VideoDecoder::pause()
{
    QThreader::pause();
}

void VideoDecoder::resume()
{
    QThreader::resume();
}

void VideoDecoder::BuildDecoder(AVCodecContext *codec_ctx,AVPacketQueue *pkt_queue,AVFrameQueue *frame_queue)
{
    if(!codec_ctx)return;
    if(pkt_queue->isEmpty())return;
    AVPacket * pkt= pkt_queue->dequeue();
    if(!pkt)return;
    int ret = avcodec_send_packet(codec_ctx, pkt);
    av_packet_free(&pkt);
    if (ret < 0)
    {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);
        qDebug() << "avcodec_send_packet failed" << errmsg;
        return ;
    }
    AVFrame* frame = av_frame_alloc();
    if(!frame)return;
    ret = avcodec_receive_frame(codec_ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
        return ;
    }else if (ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);
        qDebug() << "avcodec_receive_frame failed" << errmsg;
        return ;
    }
    frame_queue->enqueue(frame);
    av_frame_free(&frame);
}

