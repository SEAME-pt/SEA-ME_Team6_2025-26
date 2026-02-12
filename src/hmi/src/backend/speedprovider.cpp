/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @create date 2025-12-16 14:41:53
 * @modify date 2025-12-17 13:03:17
 * @desc SpeedProvider Class functions
 */

#include "speedprovider.hpp"

/**
 * @brief: Constructs an SpeedProvider object.
 *
 * Initializes the internal speed value to 0 and sets up the QObject parent.
 *
 * @param parent: (QObject *): Pointer to the parent QObject.
 * @return: void
 * @signals: None
 */
SpeedProvider::SpeedProvider(QObject *parent)
    : QObject(parent), _speedValue(0.0), _currSpeed(QString::number(0.0, 'f', 1)) {
}

/**
 * @brief: Sets the current speed, bounded between 0 and 1000.0.
 *
 * Updates the internal speed value and emits a signal if the new speed differs from the current value.
 *
 * @param speed: (double): The new speed value to set.
 * @return: void
 * @signals: speedChanged(): Emitted when the internal speed value is updated.
 */
void SpeedProvider::setSpeed(double speed) {
    speed = qBound(0.0, speed, static_cast<double>(MAX_SPEED));

    if (!qFuzzyCompare(speed, _speedValue)) {
        _speedValue = speed;
        _currSpeed = QString::number(_speedValue, 'f', 1);
        emit speedChanged();
    }
}

/**
 * @brief: Returns the current speed as a formatted string.
 *
 * Provides the current speed value as a QString.
 *
 * @param None:
 * @return: QString: Current speed as a string.
 * @signals: None
 */
QString SpeedProvider::currSpeed() const {
    return _currSpeed;
}