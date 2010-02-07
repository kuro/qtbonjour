
/**
 * @file BonjourBrowser.cpp
 * @brief BonjourBrowser implementation
 */

#include "BonjourBrowser.moc"
#include "BonjourRecord.h"

#include <QSocketNotifier>

struct BonjourBrowser::Private
{
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    QList<BonjourRecord> bonjourRecords;
    QString browsingType;

    Private () :
        dnssref(NULL),
        bonjourSocket(NULL)
    {
    }
};

BonjourBrowser::BonjourBrowser (QObject* parent) :
    QObject(parent),
    d(new Private)
{
}

BonjourBrowser::~BonjourBrowser ()
{
}

void BonjourBrowser::browseForServiceType (const QString& serviceType)
{
    DNSServiceErrorType err;
    err = DNSServiceBrowse(&d->dnssref, 0, 0,
                           serviceType.toUtf8().constData(), 0,
                           bonjourBrowseReply, this);
    if (err != kDNSServiceErr_NoError) {
        emit error(err);
    } else {
        int sockfd = DNSServiceRefSockFD(d->dnssref);
        if (sockfd == -1) {
            emit error(kDNSServiceErr_Invalid);
        } else {
            d->bonjourSocket = new QSocketNotifier(
                sockfd, QSocketNotifier::Read, this);
            connect(d->bonjourSocket, SIGNAL(activated(int)),
                    SLOT(bonjourSocketReadyRead()));
        }
    }
}

void BonjourBrowser::bonjourBrowseReply (
    DNSServiceRef, DNSServiceFlags flags, quint32, DNSServiceErrorType err,
    const char* serviceName, const char* regType, const char* replyDomain,
    void* context)
{
    BonjourBrowser* browser = static_cast<BonjourBrowser*>(context);
    if (err != kDNSServiceErr_NoError) {
        emit browser->error(err);
    } else {
        BonjourRecord record (serviceName, regType, replyDomain);
        if (flags & kDNSServiceFlagsAdd) {
            if (!browser->d->bonjourRecords.contains(record)) {
                browser->d->bonjourRecords.append(record);
            } else {
                browser->d->bonjourRecords.removeAll(record);
            }
            if (!(flags & kDNSServiceFlagsMoreComing)) {
                emit browser->currentBonjourRecordsChanged(
                    browser->d->bonjourRecords);
            }
        }
    }
}

QList<BonjourRecord> BonjourBrowser::currentRecords () const
{
    return d->bonjourRecords;
}

QString BonjourBrowser::serviceType () const
{
    return d->browsingType;
}

void BonjourBrowser::bonjourSocketReadyRead ()
{
    DNSServiceErrorType err;
    err = DNSServiceProcessResult(d->dnssref);
    if (err != kDNSServiceErr_NoError) {
        emit error(err);
    }
}
