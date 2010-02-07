
/**
 * @file BonjourRecord.h
 * @brief BonjourRecord definition
 */

#pragma once

#include <QSharedDataPointer>
#include <QVariant>

class BonjourRecord
{
public:
    BonjourRecord ();
    BonjourRecord (const QString& serviceName,
                   const QString& registeredType,
                   const QString& replyDomain);
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

private:
    struct Private;
    QSharedDataPointer<Private> d;
};

Q_DECLARE_METATYPE(BonjourRecord)
