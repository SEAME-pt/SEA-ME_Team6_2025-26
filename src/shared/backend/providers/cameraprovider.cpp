/**
 * @file cameraprovider.cpp
 * @brief Implementation of CameraProvider class
 */

#include "providers/cameraprovider.hpp"

CameraProvider::CameraProvider(QObject *parent)
    : BaseProvider(parent), _player(nullptr), _qmlSink(nullptr)
{
    qDebug() << "[CameraProvider] Initialized";
    QTimer::singleShot(500, this, &CameraProvider::initCamera);
}

void CameraProvider::setVideoSink(QVideoSink *sink)
{
    if (!sink) return;
    qDebug() << "[CameraProvider] VideoSink connected from QML";
    _qmlSink = sink;
    if (_player) _player->setVideoSink(_qmlSink);
}

void CameraProvider::initCamera()
{
    QByteArray dev = qgetenv("CAMERA_DEVICE");
    if (dev.isEmpty()) dev = "/dev/video2";

    qDebug() << "[CameraProvider] Starting v4l2 pipeline on" << dev;

    _player = new QMediaPlayer(this);

    connect(_player, &QMediaPlayer::errorOccurred, this,
        [](QMediaPlayer::Error err, const QString &msg) {
            qCritical() << "[CameraProvider] Player error:" << err << msg;
        });
    connect(_player, &QMediaPlayer::playbackStateChanged, this,
        [](QMediaPlayer::PlaybackState state) {
            qDebug() << "[CameraProvider] Playback state:" << state;
        });

    if (_qmlSink) _player->setVideoSink(_qmlSink);

    // Use QUrl with v4l2 scheme - cleaner than gst-pipeline URI
    _player->setSource(QUrl(QString("v4l2://%1").arg(QString::fromUtf8(dev))));
    _player->play();

    qDebug() << "[CameraProvider] Player started";
}