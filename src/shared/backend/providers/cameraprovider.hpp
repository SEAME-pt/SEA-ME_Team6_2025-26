#ifndef CAMERAPROVIDER_HPP
#define CAMERAPROVIDER_HPP

#include "core/baseprovider.hpp"
#include <QString>
#include <QMutex>
#include <QCamera>
#include <QMediaCaptureSession>
#include <QVideoSink>
#include <QMediaDevices>
#include <QCameraDevice>
#include <QTimer>
#include <QMediaPlayer>

/**
 * @class CameraProvider
 * @brief Handles all camera-related vehicle
 */
class CameraProvider : public BaseProvider
{
    Q_OBJECT

public:
    explicit CameraProvider(QObject *parent = nullptr);
    ~CameraProvider() override = default;

    QString providerName() const override { return "CameraProvider"; }
    Q_INVOKABLE void setVideoSink(QVideoSink *sink);

private slots:
    void initCamera();

private:
    QMediaPlayer *_player;
    QVideoSink   *_qmlSink;
};

#endif /* CAMERAPROVIDER_HPP */
