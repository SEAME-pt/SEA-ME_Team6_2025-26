/**
 * @file baseprovider.cpp
 * @brief Implementation of BaseProvider class
 */

#include "baseprovider.hpp"

BaseProvider::BaseProvider(QObject *parent)
    : QObject(parent)
{
}

void BaseProvider::emitError(const QString &message)
{
    QString fullMessage = QString("[%1] %2").arg(providerName(), message);
    qDebug() << fullMessage;
    emit errorOccurred(fullMessage);
}