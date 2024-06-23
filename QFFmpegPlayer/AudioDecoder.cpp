#include "AudioDecoder.h"
#include <memory>
#include <iostream>


AudioDecoder::AudioDecoder(QObject *parent)
    : QThreader(parent)
{
}

AudioDecoder::~AudioDecoder()
{
}
void AudioDecoder::start(Priority pri)
{
    QThreader::start(pri);
}

void AudioDecoder::stop()
{
    QThreader::stop();
}
void AudioDecoder::pause()
{
    QThreader::pause();
}

void AudioDecoder::resume()
{
    QThreader::resume();
}

AVController *AudioDecoder::initParameters(AVController * controller){
    controller->audio_codec_ctx = avcodec_alloc_context3(NULL);
    int read_ret = avcodec_parameters_to_context(controller->audio_codec_ctx, controller->audio_codecpar);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_parameters_to_context failed" << errmsg;
        avcodec_free_context(&controller->audio_codec_ctx);
        return nullptr;
    }
    // h264
    // h264_qsv  AV_CODEC_ID_H264
    //    avcodec_find_decoder_by_name()

    //    if(AV_CODEC_ID_H264 == codec_ctx_->codec_id)
    //        codec = avcodec_find_decoder_by_name("h264_qsv");
    //    else
  const  AVCodec * codec = avcodec_find_decoder(controller->audio_codec_ctx->codec_id); //作业： 硬件解码
    if(!codec) {
        qDebug() << "avcodec_find_decoder failed";
        avcodec_free_context(&controller->audio_codec_ctx);
        return nullptr;
    }

    read_ret = avcodec_open2(controller->audio_codec_ctx, codec, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_open2 failed" << errmsg;
        avcodec_free_context(&controller->audio_codec_ctx);
        return nullptr;
    }
    this->controller=controller;

    av_resample.InitAVResample(controller->audio_codec_ctx,AV_CH_LAYOUT_STEREO,44100,AV_SAMPLE_FMT_S16);
    int data_size = av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    audio_render.InitFormat(0,44100,data_size*8,2);
    return controller;
}

void AudioDecoder::freeParameters(AVController * controller){
    if(controller->audio_codec_ctx){
        avcodec_free_context(&controller->audio_codec_ctx);
        controller->audio_codec_ctx=nullptr;
    }
    av_resample.FreeAVResample();
}


void AudioDecoder::loopRunnable()
{
    if(frameFinished){
        QThreader::usleep(10);
    }
    if(state()==Running && !frameFinished){
        if (controller->get_audio_synchronize() > controller->get_master_synchronize())
        {
            QThread::usleep(10);
            return;
        }
        if(!controller->audio_pkt_queue->isEmpty()){
            if(controller->audio_frame_queue->size() <10){
                BuildDecoder(controller->audio_codec_ctx,controller->audio_pkt_queue,controller->audio_frame_queue);
            }else{
                QThread::usleep(200);
                return;
            }
        }

        if(!controller->audio_frame_queue->isEmpty()){
            AVFrame * frame= controller->audio_frame_queue->dequeue();
            if(frame){
                int64_t pts_time= (frame->pts == AV_NOPTS_VALUE) ? NAN : frame->pts;
                controller->audio_synchronize(pts_time,controller->audio_pts_begin,controller->audio_pts_base);
                QByteArray bytes= av_resample.BuiledConvert(frame);
                audio_render.WriteOutput(bytes);
                av_frame_free(&frame);
            }

        }
    }
}





void AudioDecoder::BuildDecoder(AVCodecContext *codec_ctx,AVPacketQueue *pkt_queue,AVFrameQueue *frame_queue)
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



