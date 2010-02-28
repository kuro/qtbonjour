
/**
 * @file BonjourResolver.cpp
 * @brief BonjourResolver implementation
 */

#include "BonjourResolver.moc"
#include "BonjourRecord.h"

#include <QSocketNotifier>
#include <QHostInfo>
#include <QtEndian>
#include <QBuffer>
#include <QDebug>

struct BonjourResolver::Private
{
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;

    DNSServiceErrorType errorCode;

    struct {
        QHostInfo hostInfo;
        quint16 port;
        QHash<QString, QString> text;
    } last;

    Private () :
        dnssref(NULL),
        bonjourSocket(NULL),
        errorCode(kDNSServiceErr_NoError)
    {
        last.port = 0;
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
                            (DNSServiceResolveReply)bonjourResolveReply, this);
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
        d->last.port = 0;
    }
}

void BonjourResolver::last (QHostInfo& hostInfo, quint16& port,
                            QHash<QString, QString>& text) const
{
    hostInfo = d->last.hostInfo;
    port = d->last.port;
    text = d->last.text;
}

void BonjourResolver::finishConnect (const QHostInfo& hostInfo)
{
    d->last.hostInfo = hostInfo;
    emit recordResolved(d->last.hostInfo, d->last.port, d->last.text);
    QMetaObject::invokeMethod(this, "cleanupResolve", Qt::QueuedConnection);
}

void BonjourResolver::bonjourResolveReply (
    DNSServiceRef, DNSServiceFlags, uint32_t, DNSServiceErrorType err,
    const char* fullName,
    const char* hostTarget, uint16_t port,
    uint16_t textLength, const char* textRecord,
    void* context)
{
    BonjourResolver* resolver = static_cast<BonjourResolver*>(context);
    if (err != kDNSServiceErr_NoError) {
        resolver->setError(err);
        return;
    }

    resolver->d->last.text.clear();
    QBuffer dev;
    dev.setData((char*)textRecord, textLength);
    dev.open(QIODevice::ReadOnly);
    while (!dev.atEnd()) {
        quint8 len;
        dev.read((char*)&len, sizeof(len));
        QByteArray record = dev.read(len);
        QList<QByteArray> parts = record.split('=');
        resolver->d->last.text.insert(QString::fromLocal8Bit(parts[0]),
                                      QString::fromLocal8Bit(parts[1]));
    }
    dev.close();

    resolver->d->last.port = qFromBigEndian(port);
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
