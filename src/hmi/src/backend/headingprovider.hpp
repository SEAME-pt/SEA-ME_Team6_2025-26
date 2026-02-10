/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc Class that contains heading functions and exposition to QML
 */

#ifndef headingprovider
#define headingprovider

#include <QObject>
#include <QTimer>
#include <QtMath>

class HeadingProvider : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currHeading READ currHeading NOTIFY headingChanged)

public:
    explicit HeadingProvider(QObject *parent = nullptr);

    QString currHeading() const;

public slots:
    void setHeading(double heading);

signals:
    void headingChanged();

private:
    QString _currHeading;
    double _headingValue = 0.0;
};


#endif /* headingprovider */
