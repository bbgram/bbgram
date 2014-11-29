#pragma once

#include <QObject>

class Chat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant photo READ photo NOTIFY photoChanged)
public:
    Chat();
    ~Chat();

    virtual QString title() const;
    virtual QString status() const;
    virtual QVariant photo() const;
signals:
    void titleChanged();
    void statusChanged();
    void photoChanged();
};

Q_DECLARE_METATYPE(Chat*);
