
/**
 * @file BonjourResolver.h
 * @brief BonjourResolver definition
 */

#pragma once

#include <QPair>

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

    /**
     * @brief Store the last result into the given variables.
     */
    void last (QHostInfo&, quint16&, QHash<QString, QString>&) const;

signals:
    void recordResolved (const QHostInfo& hostInfo, quint16 port,
                         const QHash<QString, QString>& txt);
    void error (DNSServiceErrorType err);

private slots:
    void bonjourSocketReadyRead ();
    void cleanupResolve ();
    void finishConnect (const QHostInfo& hostInfo);

private:
    static void DNSSD_API bonjourResolveReply (
        DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType,
        const char*, const char*, uint16_t, uint16_t, const char*, void*
        );

    void setError (DNSServiceErrorType err);

private:
    struct Private;
    QScopedPointer<Private> d;
};
