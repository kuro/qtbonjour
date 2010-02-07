
/**
 * @file BonjourResolver.cpp
 * @brief BonjourResolver implementation
 */

#include "BonjourResolver.moc"
#include "BonjourRecord.h"

#include <QSocketNotifier>
#include <QHostInfo>
#include <QDebug>

struct BonjourResolver::Private
{
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    quint16 port;

    Private () :
        dnssref(NULL),
        bonjourSocket(NULL),
        port(0)
    {
    }
};

BonjourResolver::BonjourResolver (QObject* parent) :
    QObject(parent),
    d(new Private)
{
}

BonjourResolver::~BonjourResolver ()
{
}

void BonjourResolver::resolve (const BonjourRecord& record)
{
    if (d->dnssref) {
        qWarning("Resolve already in progress");
        return;
    }

    DNSServiceErrorType err;
    err = DNSServiceResolve(&d->dnssref, 0, 0,
                            record.serviceName().toUtf8().constData(),
                            record.registeredType().toUtf8().constData(),
                            record.replyDomain().toUtf8().constData(),
                            bonjourResolveReply, this);
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

void BonjourResolver::bonjourSocketReadyRead ()
{
    DNSServiceErrorType err;
    err = DNSServiceProcessResult(d->dnssref);
    if (err != kDNSServiceErr_NoError) {
        emit error(err);
    }
}

void BonjourResolver::cleanupResolve ()
{
    if (d->dnssref) {
        DNSServiceRefDeallocate(d->dnssref);
        d->dnssref = NULL;
        delete d->bonjourSocket;
        d->port = 0;
    }
}

void BonjourResolver::finishConnect (const QHostInfo& hostInfo)
{
    emit recordResolved(hostInfo, d->port);
    QMetaObject::invokeMethod(this, "cleanupResolve", Qt::QueuedConnection);
}

void BonjourResolver::bonjourResolveReply (
    DNSServiceRef, DNSServiceFlags, quint32, DNSServiceErrorType err,
    const char*, const char* hostTarget, quint16 port, quint16, const unsigned char*, void* context)
{
    BonjourResolver* resolver = static_cast<BonjourResolver*>(context);
    if (err != kDNSServiceErr_NoError) {
        emit resolver->error(err);
        return;
    }
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    port = ((port & 0x00ff) << 8) | ((port & 0xff00) >> 8);
#endif
    resolver->d->port = port;
    QHostInfo::lookupHost(QString::fromUtf8(hostTarget),
                          resolver, SLOT(finishConnect(const QHostInfo &)));
}
