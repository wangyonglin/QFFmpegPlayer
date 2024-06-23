#include "AVFrameQueue.h"

AVFrameQueue::AVFrameQueue(QObject *parent)
    : QObject{parent}
{}
void AVFrameQueue::enqueue(AVFrame *frame) {
    QMutexLocker locker(&mutex);
    if(frame){
        if(frameQueue.size() <= this_maxSize){
            AVFrame *__frame = av_frame_alloc();
            av_frame_move_ref(__frame, frame);
            frameQueue.enqueue(__frame);
            waitCondition.wakeAll();
        }
    }

}

AVFrame* AVFrameQueue::dequeue() {
    QMutexLocker locker(&mutex);
    while (frameQueue.isEmpty()) {
        waitCondition.wait(&mutex);

    }
    return frameQueue.dequeue();
}

void AVFrameQueue::clear() {
    QMutexLocker locker(&mutex);
    while (!frameQueue.isEmpty()) {
        AVFrame *frame = frameQueue.dequeue();
        if(frame){
            av_frame_free(&frame); // 释放帧内存
        }
    }
}

int AVFrameQueue::size()
{
    return frameQueue.size();
}

bool AVFrameQueue::isEmpty()
{
    return frameQueue.isEmpty();
}

bool AVFrameQueue::isFulled()
{
    if(!frameQueue.isEmpty()){
        if(frameQueue.size()>=this_maxSize){
            return true;
        }
    }
    return false;
}
