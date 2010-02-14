
/**
 * @file BonjourRegistrar.h
 * @brief BonjourRegistrar definition
 */

#pragma once

#include <QHash>

#include <dns_sd.h>

class BonjourRecord;

class BonjourRegistrar : public QObject
{
    Q_OBJECT

public:
    BonjourRegistrar (QObject* parent = NULL);
    virtual ~BonjourRegistrar ();

    void registerService (const BonjourRecord& record, quint16 port);

    void registerService (
        const BonjourRecord& record, quint16 port,
        const QHash<QString, QString>& txt);

    DNSServiceErrorType error () const;

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

    void setError (DNSServiceErrorType err);

private:
    struct Private;
    QScopedPointer<Private> d;
};
