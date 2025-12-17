/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 15:19:29
 * @modify date 2025-12-16 15:26:00
 * @desc TimeProvider Class functions
 */

#include "timeprovider.hpp"

/**
 * @brief: Constructs a TimeProvider object, initializes the current time and date, and sets up a time to update time every second.
 *
 * @param parent: (QObject*) Pointer to the parent QObject, defaults to null.
 * @return: None
 */
TimeProvider::TimeProvider(QObject *parent): QObject(parent) {
    updateTime();
    connect(&_timer, &QTimer::timeout, this, &TimeProvider::updateTime);
    _timer.start(1000);
}

/**
 * @brief: Updates the current time and date. Emits signals if either value changes.
 *
 * @param None:
 * @return: void
 * @signals: timeChanged(): Emitted when the current time (_currTime) changes.
 * @signals: dateChanged(): Emitted when the current date (_currDate) changes.
 */
void TimeProvider::updateTime() {
    QDateTime now = QDateTime::currentDateTime();

    QString newTime = now.toString("h:mm AP");
    QString newDate = now.toString("dddd | MMMM d, yyyy");

    if (newTime != _currTime) {
        _currTime = newTime;
        emit timeChanged();
    }
    if (newDate != _currDate) {
        _currDate = newDate;
        emit dateChanged();
    }
}

/**
 * @brief: Returns the current time as a string.
 *
 * @param None:
 * @return: QString: The current time in "h:mm AP" format.
 */
QString TimeProvider::currTime() const { return _currTime; }

/**
 * @brief: Returns the current date as a string.
 *
 * @param None:
 * @return: QString: The current date in "dddd | MMMM d, yyyy" format.
 */
QString TimeProvider::currDate() const { return _currDate; }