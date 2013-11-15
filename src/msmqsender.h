#ifndef MSMQSENDER_H
#define MSMQSENDER_H

#include <QString>

class MSMQSender
{
public:
    MSMQSender();
    QString SendUtf8String(const QString &messagequeue, const QString &label, const QString &content, const bool transactional) const;
};

#endif // MSMQSENDER_H
