#pragma once

#include <QObject>
#include <QString>
#include <bb/cascades/QmlDocument>

template <typename T>
class Screen: public QObject
{
public:
    Screen(const QString& qmlAsset)
        : m_rootObject(0)
    {
        m_qmlDocument = bb::cascades::QmlDocument::create(qmlAsset).parent(this);
        m_qmlDocument->setContextProperty("_owner", this);
        m_rootObject = m_qmlDocument->createRootObject<T>();
    }

    ~Screen()
    {
    }

    T* rootObject() const
    {
        return m_rootObject;
    }

    void setContextProperty(const QString &propertyName, QObject *object)
    {
        m_qmlDocument->setContextProperty(propertyName, object);
    }
protected:
    bb::cascades::QmlDocument   *m_qmlDocument;
    T                           *m_rootObject;
};

