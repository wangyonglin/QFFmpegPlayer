#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFFmpegPlayer/QFFmpegPlayer.h>
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QFFmpegPlayer *player;

    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *event) override;
};
#endif // WIDGET_H
