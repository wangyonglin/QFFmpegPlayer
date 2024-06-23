#include "VideoDecoder.h"

VideoDecoder::VideoDecoder(QObject *parent)
    : QThreader{parent}
{}

AVController *VideoDecoder::initParameters( AVController * controller){
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
    this->controller=controller;
    return  controller;
}

void VideoDecoder::freeParameters(AVController * controller){
    if( controller->video_codec_ctx){
        avcodec_free_context(& controller->video_codec_ctx);
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
            AVFrame * frame= controller->video_frame_queue->dequeue();
            if(frame){
               int64_t pts_time= (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts;
                controller->video_synchronize(pts_time,controller->video_pts_begin,controller->video_pts_base);
                QImage image = AVFrame2RGBA8888(frame);
                emit drawImage(image);
                av_frame_free(&frame);
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

QImage VideoDecoder::AVFrame2RGBA8888(AVFrame *frame)
{
    if((!frame) || (frame->width<=0) || (frame->height<=0)){
        return QImage();
    }

    QImage img(frame->width, frame->height, QImage::Format_RGBA8888);

    SwsContext* sws_ctx = sws_getContext(frame->width,
                                         frame->height,
                                         static_cast<enum AVPixelFormat>(frame->format),
                                         frame->width,
                                         frame->height,
                                         AV_PIX_FMT_RGBA,
                                         SWS_BILINEAR,
                                         nullptr,
                                         nullptr,
                                         nullptr);


    if (!sws_ctx) {
        // 错误处理：sws_getContext 失败
        qDebug() << "sws_getContext failed.";
        return QImage();
    }

    uint8_t* data[1] = { reinterpret_cast<uint8_t*>(img.bits()) };
    int linesize[1] = { static_cast<int>(img.bytesPerLine()) };
    int ret = sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, data, linesize);
    sws_freeContext(sws_ctx);
    if (ret != frame->height) {
        // 错误处理：sws_scale 失败
        qDebug() << "sws_scale failed.";
        return QImage();
    }
    return img;
}
