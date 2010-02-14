
/**
 * @file BonjourRecord.cpp
 * @brief BonjourRecord implementation
 */

#include "BonjourRecord.h"

#include "BonjourResolver.h"

#include <QString>

struct BonjourRecord::Private : public QSharedData
{
    QString serviceName;
    QString registeredType;
    QString replyDomain;

    Private ()
    {
    }

    Private (const QString& serviceName,
             const QString& registeredType,
             const QString& replyDomain) :
        serviceName(serviceName),
        registeredType(registeredType),
        replyDomain(replyDomain)
    {
    }

};

BonjourRecord::BonjourRecord () :
    d(new Private)
{
}

BonjourRecord::BonjourRecord (const QString& serviceName,
                              const QString& registeredType,
                              const QString& replyDomain) :
    d(new Private(serviceName, registeredType, replyDomain))
{
}

BonjourRecord::BonjourRecord (const char* serviceName,
                              const char* registeredType,
                              const char* replyDomain) :
    d(new Private(QString::fromUtf8(serviceName),
                  QString::fromUtf8(registeredType),
                  QString::fromUtf8(replyDomain)))
{
}

BonjourRecord::BonjourRecord (const BonjourRecord& other) :
    d(other.d)
{
}

BonjourRecord& BonjourRecord::operator= (const BonjourRecord& other)
{
    d = other.d;
}

BonjourRecord::~BonjourRecord ()
{
}

QString BonjourRecord::serviceName () const
{
    return d->serviceName;
}

QString BonjourRecord::registeredType () const
{
    return d->registeredType;
}

QString BonjourRecord::replyDomain () const
{
    return d->replyDomain;
}

void BonjourRecord::setServiceName    (const QString& val)
{
    d->serviceName = val;
}

void BonjourRecord::setRegisteredType (const QString& val)
{
    d->registeredType = val;
}

void BonjourRecord::setReplyDomain  (const QString& val)
{
    d->replyDomain = val;
}

bool BonjourRecord::operator== (const BonjourRecord& other) const
{
    return
        d->serviceName == other.serviceName() &&
        d->registeredType == other.registeredType() &&
        d->replyDomain == other.replyDomain()
        ;
}

void BonjourRecord::resolve (QObject* receiver,
                             const char* member,
                             const char* errorMember)
{
    BonjourResolver* resolver = NULL;
    resolver = new BonjourResolver(receiver);
    Q_CHECK_PTR(resolver);

    QObject::connect(resolver,
                     SIGNAL(recordResolved(const QHostInfo&, quint16,
                                           const QHash<QString, QString>&)),
                     receiver,
                     member);

    if (errorMember) {
        QObject::connect(resolver,
                         SIGNAL(error(DNSServiceErrorType)),
                         receiver,
                         errorMember);
    }

    // clean up in the future
    QObject::connect(resolver,
                     SIGNAL(error(DNSServiceErrorType)),
                     resolver,
                     SLOT(deleteLater()));

    QObject::connect(resolver,
                     SIGNAL(recordResolved(const QHostInfo&, quint16,
                                           const QHash<QString, QString>&)),
                     resolver,
                     SLOT(deleteLater()));

    // resolve
    resolver->resolve(*this);
}
