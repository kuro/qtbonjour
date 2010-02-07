
/**
 * @file BonjourBrowser.h
 * @brief BonjourBrowser definition
 */

#pragma once

#include <QObject>

#include <dns_sd.h>

class BonjourRecord;

class BonjourBrowser : public QObject
{
    Q_OBJECT

public:
    BonjourBrowser (QObject* parent = NULL);
    virtual ~BonjourBrowser ();

    void browseForServiceType (const QString& serviceType);
    QList<BonjourRecord> currentRecords () const;
    QString serviceType () const;

signals:
    void currentBonjourRecordsChanged (const QList<BonjourRecord>& list);
    void error (DNSServiceErrorType err);

private slots:
    void bonjourSocketReadyRead ();

private:
    static void DNSSD_API bonjourBrowseReply (
        DNSServiceRef, DNSServiceFlags, quint32, DNSServiceErrorType,
        const char*, const char*, const char*, void*);

private:
    struct Private;
    QScopedPointer<Private> d;
};
