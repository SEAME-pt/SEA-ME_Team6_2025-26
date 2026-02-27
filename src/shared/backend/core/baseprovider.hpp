/**
 * @file baseprovider.hpp
 * @brief Base class for all vehicle data providers
 * @desc Provides common functionality for signal providers
 */

#ifndef BASEPROVIDER_HPP
#define BASEPROVIDER_HPP

#include <QObject>
#include <QDebug>
#include <QString>
#include <QMutex>

/**
 * @class BaseProvider
 * @brief Abstract base class for all vehicle signal providers
 */
class BaseProvider : public QObject
{
    Q_OBJECT

public:
    explicit BaseProvider(QObject *parent = nullptr);
    virtual ~BaseProvider() = default;

    /**
     * @brief Get the provider name for debugging
     */
    virtual QString providerName() const = 0;

signals:
    /**
     * @brief Emitted when provider encounters an error
     */
    void errorOccurred(QString error);

protected:
    /**
     * @brief Thread-safe mutex for data access
     */
    mutable QMutex _mutex;

    /**
     * @brief Helper to emit errors with provider context
     */
    void emitError(const QString &message);
};

#endif /* BASEPROVIDER_HPP */