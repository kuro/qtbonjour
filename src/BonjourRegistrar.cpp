
/**
 * @file BonjourRegistrar.cpp
 * @brief BonjourRegistrar implementation
 */

#include "BonjourRegistrar.moc"
#include "BonjourRecord.h"

#include <QSocketNotifier>
#include <QtEndian>
#include <QHashIterator>
#include <QBuffer>
#include <QDebug>

struct BonjourRegistrar::Private
{
    DNSServiceRef dnssref;
    QSocketNotifier *bonjourSocket;
    BonjourRecord finalRecord;

    DNSServiceErrorType errorCode;

    Private () :
        dnssref(NULL),
        bonjourSocket(NULL),
        errorCode(kDNSServiceErr_NoError)
    {
    }
};

BonjourRegistrar::BonjourRegistrar (QObject* parent) :
    QObject(parent),
    d(new Private)
{
}

BonjourRegistrar::~BonjourRegistrar ()
{
}

void BonjourRegistrar::registerService (const BonjourRecord& record, quint16 port)
{
    QHash<QString, QString> txt;
    registerService(record, port, txt);
}

void BonjourRegistrar::registerService (
    const BonjourRecord& record, quint16 port,
    const QHash<QString, QString>& txt)
{
    if (d->dnssref) {
        qWarning("Warning: Already registered a service for this object, aborting new register");
        return;
    }

    DNSServiceErrorType err;

    QByteArray buf;
    QBuffer dev (&buf);
    dev.open(QIODevice::WriteOnly);
    QHashIterator<QString, QString> it (txt);
    while (it.hasNext()) {
        it.next();
        QString key = it.key();
        QString value = it.value();
        quint8 len = key.size() + value.size() + 1;
        dev.write((char*)&len, sizeof(len));
        dev.write(it.key().toLocal8Bit());
        dev.write("=");
        dev.write(it.value().toLocal8Bit());
    }
    dev.close();

    err = DNSServiceRegister(
        &d->dnssref, 0, 0,
        record.serviceName().toUtf8().constData(),
        record.registeredType().toUtf8().constData(),
        record.replyDomain().isEmpty()
            ? 0 : record.replyDomain().toUtf8().constData(),
        0,
        qToBigEndian(port),
        buf.size(), buf.data(),
        bonjourRegisterService, this);

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

void BonjourRegistrar::bonjourSocketReadyRead ()
{
    DNSServiceErrorType err;
    err = DNSServiceProcessResult(d->dnssref);
    if (err != kDNSServiceErr_NoError) {
        setError(err);
    }
}

void BonjourRegistrar::bonjourRegisterService (
    DNSServiceRef sdRef, DNSServiceFlags,
    DNSServiceErrorType err, const char *name,
    const char *regtype, const char *domain,
    void *context
    )
{
    BonjourRegistrar* registrar = static_cast<BonjourRegistrar*>(context);
    if (err != kDNSServiceErr_NoError) {
        registrar->setError(err);
    } else {
        registrar->d->finalRecord
            = BonjourRecord(QString::fromUtf8(name),
                            QString::fromUtf8(regtype),
                            QString::fromUtf8(domain));
        emit registrar->serviceRegistered(registrar->d->finalRecord);
    }
}

DNSServiceErrorType BonjourRegistrar::error () const
{
    return d->errorCode;
}

void BonjourRegistrar::setError (DNSServiceErrorType err)
{
    d->errorCode = err;
    emit error(err);
}
