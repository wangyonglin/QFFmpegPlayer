#include "Widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent),
    player(new QFFmpegPlayer(this))
{
    resize(800,600);
    player->play("/home/wangyonglin/视频/713991376-1-192.mp4");
}

Widget::~Widget() {
    player->free();
    delete player;
}

void Widget::resizeEvent(QResizeEvent *event)
{
    player->resize(event->size());
}
