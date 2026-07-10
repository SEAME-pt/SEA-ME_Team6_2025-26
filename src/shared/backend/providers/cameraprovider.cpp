#include "providers/cameraprovider.hpp"
#include <QVideoFrame>
#include <QImage>
#include <gst/video/video.h>

CameraProvider::CameraProvider(QObject *parent)
    : BaseProvider(parent), _qmlSink(nullptr), _pipeline(nullptr),
      _appsink(nullptr), _frameTimer(nullptr)
{
    QByteArray serverEnv = qgetenv("JETRACER_SERVER");
    if (!serverEnv.isEmpty()) _server = QString::fromUtf8(serverEnv);

    QByteArray portEnv = qgetenv("CAMERA_PORT");
    if (!portEnv.isEmpty()) _port = portEnv.toInt();

    gst_init(nullptr, nullptr);

    qDebug() << "[CameraProvider] Initialized — stream:" << _server << ":" << _port;
    QTimer::singleShot(500, this, &CameraProvider::initCamera);
}

CameraProvider::~CameraProvider()
{
    if (_frameTimer) _frameTimer->stop();
    if (_pipeline) {
        gst_element_set_state(_pipeline, GST_STATE_NULL);
        gst_object_unref(_pipeline);
    }
}

void CameraProvider::setVideoSink(QVideoSink *sink)
{
    if (!sink) return;
    qDebug() << "[CameraProvider] VideoSink connected from QML";
    _qmlSink = sink;
}

void CameraProvider::initCamera()
{
    // QString pipelineStr = QString(
    //     "udpsrc port=%1 caps=\"image/jpeg,width=1280,height=720,framerate=30/1\" "
    //     "! jpegdec ! videoconvert "
    //     "! video/x-raw,format=BGRx "
    //     "! appsink name=appsink0 emit-signals=false max-buffers=1 drop=true sync=false"
    // ).arg(_port);

    // qDebug() << "[CameraProvider] Pipeline:" << pipelineStr;

    // GError *err = nullptr;
    // _pipeline = gst_parse_launch(pipelineStr.toUtf8().constData(), &err);

    // if (err) {
    //     qCritical() << "[CameraProvider] Pipeline error:" << err->message;
    //     g_error_free(err);
    //     return;
    // }

    // _appsink = gst_bin_get_by_name(GST_BIN(_pipeline), "appsink0");
    // if (!_appsink) {
    //     qCritical() << "[CameraProvider] Could not find appsink";
    //     return;
    // }

    // GstStateChangeReturn ret = gst_element_set_state(_pipeline, GST_STATE_PLAYING);
    // if (ret == GST_STATE_CHANGE_FAILURE) {
    //     qCritical() << "[CameraProvider] Failed to start pipeline";
    //     return;
    // }

    // qDebug() << "[CameraProvider] Pipeline running";

    // // Poll frames on main thread at 30fps
    // _frameTimer = new QTimer(this);
    // connect(_frameTimer, &QTimer::timeout, this, &CameraProvider::pullFrame);
    // _frameTimer->start(33);
}

void CameraProvider::pullFrame()
{
    // if (!_appsink || !_qmlSink) return;

    // GstSample *sample = gst_app_sink_try_pull_sample(GST_APP_SINK(_appsink), 0);
    // if (!sample) return;

    // GstBuffer *buffer = gst_sample_get_buffer(sample);
    // GstCaps   *caps   = gst_sample_get_caps(sample);

    // GstVideoInfo info;
    // gst_video_info_from_caps(&info, caps);

    // GstMapInfo map;
    // if (gst_buffer_map(buffer, &map, GST_MAP_READ)) {
    //     const int width  = GST_VIDEO_INFO_WIDTH(&info);
    //     const int height = GST_VIDEO_INFO_HEIGHT(&info);
    //     const int stride = GST_VIDEO_INFO_PLANE_STRIDE(&info, 0);

    //     QVideoFrameFormat format(
    //         QSize(width, height),
    //         QVideoFrameFormat::Format_BGRX8888
    //     );

    //     QVideoFrame frame(format);

    //     if (frame.map(QVideoFrame::WriteOnly)) {
    //         // Copy row-by-row to handle stride differences
    //         const int copyStride = qMin(stride, frame.bytesPerLine(0));
    //         const int gstStride = GST_VIDEO_INFO_PLANE_STRIDE(&info, 0);
    //         const int qtStride  = frame.bytesPerLine(0);
    //         const int copyBytes = qMin(gstStride, qtStride);

    //         for (int y = 0; y < height; ++y) {
    //             memcpy(
    //                 frame.bits(0) + y * qtStride,
    //                 map.data      + y * gstStride,
    //                 copyBytes
    //             );
    //         }
    //                     frame.unmap();
    //     }

    //     _qmlSink->setVideoFrame(frame);
    //     gst_buffer_unmap(buffer, &map);
    // }

    // gst_sample_unref(sample);
}