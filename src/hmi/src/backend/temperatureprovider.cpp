/**
 * @author souzitaaaa
 * @email diogo.nogueirasousa123@gmail.com
 * @desc TemperatureProvider Class functions
 */

#include "temperatureprovider.hpp"

/**
 * @brief Constructs a new TemperatureProvider object and initializes the current temperature.
 * 
 * @param parent: (QObject *) Pointer to the parent QObject, defaults to nullptr.
 * @return: None
 */
TemperatureProvider::TemperatureProvider(QObject *parent)
    : QObject(parent), _currTemperature("0") {
}

/**
 * @brief: Sets the temperature to the specified value if it differs from the current value.
 *         Emits a signal if the temperature is updated.
 *
 * @param temperature: (double) The new temperature value to set.
 * @return: void
 * @signals: temperatureChanged() Emitted when the temperature value (_temperatureValue) changes.
 */
void TemperatureProvider::setTemperature(double temperature) {

    if (!qFuzzyCompare(temperature, _temperatureValue)) {
        _temperatureValue = temperature;
        _currTemperature = QString::number(_temperatureValue, 'f', 1);
        emit temperatureChanged();
    }
}

/**
 * @brief: Returns the current temperature as a formatted string.
 *
 * @param None:
 * @return: QString The current temperature as a string.
 */
QString TemperatureProvider::currTemperature() const {
    return _currTemperature;
}
