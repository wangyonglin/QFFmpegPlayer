#include "QFFmpeg.h"



QFFmpeg::QFFmpegDecode *QFFmpeg::OpenInitDecode(AVCodecParameters *codecpar)
{
    QFFmpeg::QFFmpegDecode * ffmpegDecode = new QFFmpeg::QFFmpegDecode();
    if(!ffmpegDecode)return nullptr;
    ffmpegDecode->codec_ctx = avcodec_alloc_context3(NULL);
    if(!ffmpegDecode->codec_ctx){
        delete ffmpegDecode;
        return nullptr;
    }
    int read_ret = avcodec_parameters_to_context(ffmpegDecode->codec_ctx, codecpar);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_parameters_to_context failed" << errmsg;
        avcodec_free_context(&ffmpegDecode->codec_ctx);
        delete ffmpegDecode;
        return nullptr;
    }
    // h264
    // h264_qsv  AV_CODEC_ID_H264
    //    avcodec_find_decoder_by_name()

    //    if(AV_CODEC_ID_H264 == codec_ctx_->codec_id)
    //        codec = avcodec_find_decoder_by_name("h264_qsv");
    //    else
    ffmpegDecode->codec = avcodec_find_decoder(ffmpegDecode->codec_ctx->codec_id); //作业： 硬件解码
    if(!ffmpegDecode->codec) {
        qDebug() << "avcodec_find_decoder failed";
        avcodec_free_context(&ffmpegDecode->codec_ctx);
        delete ffmpegDecode;
        return nullptr;
    }

    read_ret = avcodec_open2(ffmpegDecode->codec_ctx, ffmpegDecode->codec, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_open2 failed" << errmsg;
        avcodec_free_context(&ffmpegDecode->codec_ctx);
        delete ffmpegDecode;
        return nullptr;
    }
    return ffmpegDecode;
}

void QFFmpeg::CloseInitDecode(QFFmpeg::QFFmpegDecode * ffmpegDecode)
{
    if(ffmpegDecode->codec_ctx){
        avcodec_close(ffmpegDecode->codec_ctx);
        avcodec_free_context(&ffmpegDecode->codec_ctx);
        ffmpegDecode->codec_ctx=nullptr;
        delete ffmpegDecode;
    }
}

QFFmpeg::QFFmpegDemux *QFFmpeg::OpenInitDemux(QString url)
{
    QFFmpeg::QFFmpegDemux * ffmpegDemux=new QFFmpeg::QFFmpegDemux();
    if(!ffmpegDemux)return nullptr;
    ffmpegDemux->strurl=url.toLocal8Bit().data();
    if(  ffmpegDemux->ifmt_ctx)avformat_close_input(&  ffmpegDemux->ifmt_ctx);
    int read_ret = avformat_open_input(&  ffmpegDemux->ifmt_ctx,   ffmpegDemux->strurl, NULL, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avformat_open_input failed" << errmsg;
        delete ffmpegDemux;
        return nullptr;
    }

    read_ret = avformat_find_stream_info(  ffmpegDemux->ifmt_ctx, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avformat_find_stream_info failed" << errmsg;
        avformat_close_input(&  ffmpegDemux->ifmt_ctx);
        delete ffmpegDemux;
        return nullptr;
    }
    av_dump_format(  ffmpegDemux->ifmt_ctx, 0,   ffmpegDemux->strurl, 0);
    ffmpegDemux-> audio_stream_index=av_find_best_stream(  ffmpegDemux->ifmt_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, NULL, 0);
    ffmpegDemux->video_stream_index=av_find_best_stream(  ffmpegDemux->ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if(ffmpegDemux->audio_stream_index>=0){
        ffmpegDemux->audio_codecpar=  ffmpegDemux->ifmt_ctx->streams[ffmpegDemux->audio_stream_index]->codecpar;
        ffmpegDemux->audio_pts_base=  ffmpegDemux->ifmt_ctx->streams[ffmpegDemux->audio_stream_index]->time_base;
        ffmpegDemux-> audio_pts_begin = ffmpegDemux->ifmt_ctx->streams[ffmpegDemux->audio_stream_index]->start_time;

        if(  ffmpegDemux->video_stream_index>=0){
            ffmpegDemux->video_codecpar=  ffmpegDemux->ifmt_ctx->streams[ffmpegDemux->video_stream_index]->codecpar;
            ffmpegDemux->video_pts_base=  ffmpegDemux->ifmt_ctx->streams[ffmpegDemux->video_stream_index]->time_base;
            ffmpegDemux-> video_pts_begin = ffmpegDemux->ifmt_ctx->streams[ffmpegDemux->video_stream_index]->start_time;
        }
        qInfo() << ("OpenInitDemux Init leave");
        return ffmpegDemux;
    }
}
void QFFmpeg::CloseInitDemux(QFFmpegDemux *ffmpegDemux)
{
    if(ffmpegDemux){
        if(ffmpegDemux->ifmt_ctx)avformat_close_input(&ffmpegDemux->ifmt_ctx);
        delete ffmpegDemux;
    }
}



AVFrame* QFFmpeg::ReceiveAVFrame(AVCodecContext *codec_ctx, const AVPacket *avpkt)
{
    if(!codec_ctx)return nullptr;
    int ret = avcodec_send_packet(codec_ctx, avpkt);
    if (ret < 0)
    {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);
        qDebug() << "avcodec_send_packet failed" << errmsg;
        return nullptr;
    }
    AVFrame* frame = av_frame_alloc();
    ret = avcodec_receive_frame(codec_ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
        return nullptr;
    }else if (ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);

        qDebug() << "avcodec_receive_frame failed" << errmsg;
        return nullptr;
    }
    return frame;
}

QFFmpeg::QFFmpegResample *QFFmpeg::InitResampler(AVCodecContext *codec_ctx,
                                                 int64_t dst_sample_rate,
                                                 AVSampleFormat dst_sample_fmt,
                                                 int64_t dst_ch_layout)
{
    QFFmpeg::QFFmpegResample * ffmpegResample=new QFFmpeg::QFFmpegResample();
    if(!ffmpegResample)return nullptr;
    ffmpegResample->dec_ctx=codec_ctx;
    ffmpegResample->dst_sample_rate=dst_sample_rate;
    ffmpegResample->dst_sample_fmt=dst_sample_fmt;
    ffmpegResample->dst_ch_layout=dst_ch_layout;
    ffmpegResample->dst_sample_bytes=av_get_bytes_per_sample(ffmpegResample->dst_sample_fmt);


    int ret =0;
    /* create resampler context */
    ffmpegResample->swr_ctx = swr_alloc();
    if (!ffmpegResample->swr_ctx) {
        qDebug() << "Could not allocate resampler context";
        ret = AVERROR(ENOMEM);
        delete ffmpegResample;
        return nullptr;
    }
    if (ffmpegResample->dec_ctx->channel_layout == 0)
    {
        ffmpegResample->dec_ctx->channel_layout = av_get_default_channel_layout(ffmpegResample->dec_ctx->channels);
    }
    /* set options */
    av_opt_set_int(ffmpegResample->swr_ctx, "in_channel_layout",   ffmpegResample->dec_ctx->channel_layout, 0);
    av_opt_set_int(ffmpegResample->swr_ctx, "in_sample_rate",      ffmpegResample->dec_ctx->sample_rate, 0);
    av_opt_set_sample_fmt(ffmpegResample->swr_ctx, "in_sample_fmt",ffmpegResample->dec_ctx->sample_fmt, 0);

    av_opt_set_int(ffmpegResample->swr_ctx, "out_channel_layout",    ffmpegResample->dst_ch_layout, 0);     //AV_CH_LAYOUT_STEREO
    av_opt_set_int(ffmpegResample->swr_ctx, "out_sample_rate",      ffmpegResample->dst_sample_rate, 0);   //44100
    av_opt_set_sample_fmt(ffmpegResample->swr_ctx, "out_sample_fmt",ffmpegResample->dst_sample_fmt, 0);    //AV_SAMPLE_FMT_S16

    /* initialize the resampling context */
    if ((ret = swr_init(ffmpegResample->swr_ctx)) < 0) {
        qDebug() << "Failed to initialize the resampling context";
        swr_free(&ffmpegResample->swr_ctx);
        return nullptr;
    }

    return ffmpegResample;
}


QByteArray QFFmpeg::BuildResampler(QFFmpegResample * ffmpegResample,AVFrame *frame)
{
    QByteArray byteBuffer;
    if(!ffmpegResample)return byteBuffer;
    if(!frame) return byteBuffer;
    // 转码音频帧
    // 计算转码后的音频数据大小
    int dstNbSamples = av_rescale_rnd(
        swr_get_delay(ffmpegResample->swr_ctx,ffmpegResample->dst_sample_rate) + frame->nb_samples,
        ffmpegResample->dst_sample_rate,ffmpegResample->dst_sample_rate, AV_ROUND_UP);

    int dstBufferSize = av_samples_get_buffer_size(nullptr, 2, dstNbSamples,ffmpegResample->dst_sample_fmt, 0);
    if(dstBufferSize<=0) return byteBuffer;

    QByteArray outputBuffer(dstBufferSize, 0);

    // 分配转码后的音频数据缓冲区
    uint8_t *outputBufferData = reinterpret_cast<uint8_t*>(outputBuffer.data());


    // 进行音频转码
    int numSamples = swr_convert(ffmpegResample->swr_ctx,
                                 &outputBufferData,
                                 dstNbSamples,
                                 const_cast<const uint8_t **>(frame->data),
                                 frame->nb_samples);
    if (numSamples < 0) {
        qDebug() << "音频转码失败";
        av_freep(&outputBufferData);
    }
    else{
        byteBuffer.resize(dstBufferSize);
        return outputBuffer;

    }


    return byteBuffer;;
}

void QFFmpeg::FreeResampler(QFFmpegResample *ffmpegResample)
{
    if(!ffmpegResample)return;
    if(ffmpegResample->swr_ctx){
        swr_free(&ffmpegResample->swr_ctx);
        ffmpegResample->swr_ctx=nullptr;
    }
    delete ffmpegResample;
}


QImage QFFmpeg::AVFrame2RGBA8888(AVFrame *frame)
{
    if((!frame) || (frame->width<=0) || (frame->height<=0)){
        return QImage();
    }

    QImage img(frame->width, frame->height, QImage::Format_RGBA8888);

     struct SwsContext* sws_ctx = sws_getContext(frame->width,
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

AVCodecContext * QFFmpeg::BuildDecoder(AVCodecParameters *codecpar)
{
    AVCodecContext *codec_ctx = avcodec_alloc_context3(NULL);
    if(!codec_ctx){
        return nullptr;
    }
    int read_ret = avcodec_parameters_to_context(codec_ctx, codecpar);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_parameters_to_context failed" << errmsg;
        avcodec_free_context(&codec_ctx);
        return nullptr;
    }
    // h264
    // h264_qsv  AV_CODEC_ID_H264
    //    avcodec_find_decoder_by_name()

    //    if(AV_CODEC_ID_H264 == codec_ctx_->codec_id)
    //        codec = avcodec_find_decoder_by_name("h264_qsv");
    //    else
  const AVCodec * codec = avcodec_find_decoder(codec_ctx->codec_id); //作业： 硬件解码
    if(!codec) {
        qDebug() << "avcodec_find_decoder failed";
        avcodec_free_context(&codec_ctx);
        return nullptr;
    }

    read_ret = avcodec_open2(codec_ctx, codec, NULL);
    if(read_ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, read_ret);
        qDebug() << "avcodec_open2 failed" << errmsg;
        avcodec_free_context(&codec_ctx);
        return nullptr;
    }
    return codec_ctx;
}

void QFFmpeg::FreeDecoder(AVCodecContext *codec_ctx)
{
    if(codec_ctx){
        avcodec_free_context(&codec_ctx);
        codec_ctx=nullptr;
    }
}

void QFFmpeg::ReceiveDecoder(AVCodecContext *codec_ctx,AVPacketQueue *pkt_queue,AVFrameQueue *frame_queue)
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

AVFrame * QFFmpeg::ReceiveAVFrame(AVCodecContext *codec_ctx,AVPacketQueue *pkt_queue)
{
    if(!codec_ctx)return nullptr;
    if(pkt_queue->isEmpty())return nullptr;
    AVPacket * pkt= pkt_queue->dequeue();
    if(!pkt)return nullptr;
    int ret = avcodec_send_packet(codec_ctx, pkt);
    av_packet_free(&pkt);
    if (ret < 0)
    {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);
        qDebug() << "avcodec_send_packet failed" << errmsg;
        return nullptr;
    }
    AVFrame* frame = av_frame_alloc();
    if(!frame)return nullptr;
    ret = avcodec_receive_frame(codec_ctx, frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF){
        return nullptr;
    }else if (ret < 0) {
        char errmsg[AV_ERROR_MAX_STRING_SIZE];
        av_make_error_string(errmsg,AV_ERROR_MAX_STRING_SIZE, ret);
        qDebug() << "avcodec_receive_frame failed" << errmsg;
        return nullptr;
    }
  //  av_frame_free(&frame);
    return frame;
}

void QFFmpeg::FreeReceiveContext(AVCodecContext *codec_ctx){
    if(codec_ctx) avcodec_free_context(&codec_ctx);
}
