
/**
 * @file BonjourResolver.h
 * @brief BonjourResolver definition
 */

#pragma once

#include <QObject>

#include <dns_sd.h>

class QHostInfo;

class BonjourRecord;

class BonjourResolver : public QObject
{
    Q_OBJECT

public:
    BonjourResolver (QObject* parent = NULL);
    virtual ~BonjourResolver ();

    void resolve (const BonjourRecord& record);

    DNSServiceErrorType error () const;

signals:
    void recordResolved (const QHostInfo& hostInfo, quint16 port);
    void error (DNSServiceErrorType err);

private slots:
    void bonjourSocketReadyRead ();
    void cleanupResolve ();
    void finishConnect (const QHostInfo& hostInfo);

private:
    static void DNSSD_API bonjourResolveReply (
        DNSServiceRef, DNSServiceFlags, quint32, DNSServiceErrorType,
        const char*, const char*, quint16, quint16, const unsigned char*, void*
        );

    void setError (DNSServiceErrorType err);

private:
    struct Private;
    QScopedPointer<Private> d;
};
