
/**
 * @file BonjourResolver.cpp
 * @brief BonjourResolver implementation
 */

#include "BonjourResolver.moc"
#include "BonjourRecord.h"

#include <QSocketNotifier>
#include <QHostInfo>
#include <QtEndian>
#include <QDebug>

struct BonjourResolver::Private
{
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    quint16 port;

    QPair<QHostInfo, quint16> last;

    DNSServiceErrorType errorCode;

    Private () :
        dnssref(NULL),
        bonjourSocket(NULL),
        port(0),
        errorCode(kDNSServiceErr_NoError)
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
        setError(err);
    } else {
        int sockfd = DNSServiceRefSockFD(d->dnssref);
        if (sockfd == -1) {
            setError(kDNSServiceErr_Invalid);
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
        setError(err);
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

QPair<QHostInfo, quint16> BonjourResolver::lastResult () const
{
    return d->last;
}

void BonjourResolver::finishConnect (const QHostInfo& hostInfo)
{
    d->last = qMakePair(hostInfo, d->port);
    emit recordResolved(hostInfo, d->port);
    QMetaObject::invokeMethod(this, "cleanupResolve", Qt::QueuedConnection);
}

void BonjourResolver::bonjourResolveReply (
    DNSServiceRef, DNSServiceFlags, quint32, DNSServiceErrorType err,
    const char*, const char* hostTarget, quint16 port, quint16, const unsigned char*, void* context)
{
    BonjourResolver* resolver = static_cast<BonjourResolver*>(context);
    if (err != kDNSServiceErr_NoError) {
        resolver->setError(err);
        return;
    }
    resolver->d->port = qFromBigEndian(port);
    QHostInfo::lookupHost(QString::fromUtf8(hostTarget),
                          resolver, SLOT(finishConnect(const QHostInfo &)));
}

DNSServiceErrorType BonjourResolver::error () const
{
    return d->errorCode;
}

void BonjourResolver::setError (DNSServiceErrorType err)
{
    d->errorCode = err;
    emit error(err);
}
