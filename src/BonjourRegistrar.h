
/**
 * @file BonjourRegistrar.h
 * @brief BonjourRegistrar definition
 */

#pragma once

#include <QObject>

#include <dns_sd.h>

class BonjourRecord;

class BonjourRegistrar : public QObject
{
    Q_OBJECT

public:
    BonjourRegistrar (QObject* parent = NULL);
    virtual ~BonjourRegistrar ();

    void registerService (const BonjourRecord& record, quint16 port);

signals:
    void serviceRegistered (const BonjourRecord& record);
    void error (DNSServiceErrorType err);

private slots:
    void bonjourSocketReadyRead ();

private:
    static void DNSSD_API bonjourRegisterService (
        DNSServiceRef, DNSServiceFlags, DNSServiceErrorType,
        const char*, const char*, const char*, void*
        );

private:
    struct Private;
    QScopedPointer<Private> d;
};
