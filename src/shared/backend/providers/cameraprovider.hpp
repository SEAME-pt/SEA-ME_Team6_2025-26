#ifndef CAMERAPROVIDER_HPP
#define CAMERAPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QVideoSink>
#include <QTimer>
#include <QThread>
#include <QString>
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video.h>

class CameraProvider : public BaseProvider
{
    Q_OBJECT

public:
    explicit CameraProvider(QObject *parent = nullptr);
    ~CameraProvider() override;

    QString providerName() const override { return "CameraProvider"; }
    Q_INVOKABLE void setVideoSink(QVideoSink *sink);

private slots:
    void initCamera();
    void pullFrame();

private:
    QVideoSink  *_qmlSink;
    GstElement  *_pipeline;
    GstElement  *_appsink;
    QTimer      *_frameTimer;

    QString _server = "10.21.220.191";
    int     _port   = 5600;
};

#endif /* CAMERAPROVIDER_HPP */