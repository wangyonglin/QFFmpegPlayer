#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMessageBox>

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

    // QWidget interface
protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
};
#endif // WIDGET_H
