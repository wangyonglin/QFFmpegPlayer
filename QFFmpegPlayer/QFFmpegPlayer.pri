contains(DEFINES, unix)
{
    INCLUDEPATH +=\
        $$PWD/include

    LIBS += -lavcodec
    LIBS += -lavdevice
    LIBS += -lavfilter
    LIBS += -lavformat
    LIBS += -lavutil
    LIBS += -lswscale
    LIBS += -lswresample
    LIBS  += -lcblas
    LIBS  += -Wall -lpthread
}


HEADERS += \
    $$PWD/AVControllerFFmpeg.h \
    $$PWD/AVDemuxer.h \
    $$PWD/AVFrameQueue.h \
    $$PWD/AVPacketQueue.h \
    $$PWD/AVResample.h \
    $$PWD/AVSynchronize.h \
    $$PWD/AudioDecoder.h \
    $$PWD/AudioRender.h \
    $$PWD/QFFmpeg.h \
    $$PWD/QFFmpegPlayer.h \
    $$PWD/QImageUitls.h \
    $$PWD/QThreader.h \
    $$PWD/VideoDecoder.h

SOURCES += \
    $$PWD/AVControllerFFmpeg.cpp \
    $$PWD/AVDemuxer.cpp \
    $$PWD/AVFrameQueue.cpp \
    $$PWD/AVPacketQueue.cpp \
    $$PWD/AVResample.cpp \
    $$PWD/AVSynchronize.cpp \
    $$PWD/AudioDecoder.cpp \
    $$PWD/AudioRender.cpp \
    $$PWD/QFFmpeg.cpp \
    $$PWD/QFFmpegPlayer.cpp \
    $$PWD/QImageUitls.cpp \
    $$PWD/QThreader.cpp \
    $$PWD/VideoDecoder.cpp

