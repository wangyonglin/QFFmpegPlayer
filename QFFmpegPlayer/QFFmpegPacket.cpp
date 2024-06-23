#include "AVPacketQueue.h"

AVPacketQueue::AVPacketQueue(QObject *parent)
    : QObject{parent}
{}
void AVPacketQueue::enqueue(AVPacket *packet) {
    QMutexLocker locker(&mutex);
    if(packetQueue.size() <= this_maxSize){
        AVPacket *__packet = av_packet_alloc();
        av_packet_move_ref(__packet, packet);
        packetQueue.enqueue(__packet);
        waitCondition.wakeAll();
    }
}

AVPacket* AVPacketQueue::dequeue() {
    QMutexLocker locker(&mutex);
    while (packetQueue.isEmpty()) {
        waitCondition.wait(&mutex);

    }
    return packetQueue.dequeue();
}

void AVPacketQueue::clear() {
    QMutexLocker locker(&mutex);
    while (!packetQueue.isEmpty()) {
        AVPacket *frame = packetQueue.dequeue();
        if(frame){
            av_packet_free(&frame); // 释放帧内存
        }
    }
}

int AVPacketQueue::size()
{
    return packetQueue.size();
}

bool AVPacketQueue::isEmpty()
{
    return packetQueue.isEmpty();
}

bool AVPacketQueue::isFulled()
{
    if(!packetQueue.isEmpty()){
        if(packetQueue.size()>=this_maxSize){
            return true;
        }
    }
    return false;
}
