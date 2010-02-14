
/**
 * @file BonjourRecord.h
 * @brief BonjourRecord definition
 */

#pragma once

#include <QSharedDataPointer>
#include <QVariant>

#include <dns_sd.h>

class QHostInfo;

class BonjourRecord
{
public:
    BonjourRecord ();
    BonjourRecord (const QString& serviceName,
                   const QString& registeredType,
                   const QString& replyDomain = QString());
    BonjourRecord (const char* serviceName,
                   const char* registeredType,
                   const char* replyDomain);
    BonjourRecord (const BonjourRecord& serviceName);
    ~BonjourRecord ();

    QString serviceName () const;
    QString registeredType () const;
    QString replyDomain () const;

    void setServiceName    (const QString& val);
    void setRegisteredType (const QString& val);
    void setReplyDomain    (const QString& val);

    bool operator== (const BonjourRecord& other) const;
    BonjourRecord& operator= (const BonjourRecord& other);

    void resolve (QObject* receiver,
                  const char* member,
                  const char* errorMember = NULL);

private:
    struct Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(BonjourRecord)
