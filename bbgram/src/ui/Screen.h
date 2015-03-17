#pragma once

#include <QObject>
#include <QString>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>

class Screen: public QObject
{
public:
    Screen()
        : m_rootObject(0)
    {
    }

    ~Screen()
    {
    }

    bb::cascades::AbstractPane* rootObject() const
    {
        return m_rootObject;
    }

    void setContextProperty(const QString &propertyName, QObject *object)
    {
        m_qmlDocument->setContextProperty(propertyName, object);
    }
protected:
    bb::cascades::QmlDocument   *m_qmlDocument;
    bb::cascades::AbstractPane  *m_rootObject;
};

