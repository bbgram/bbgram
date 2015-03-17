#pragma once

#include <QObject>
#include <QtDeclarative/qdeclarative.h>

#include <bb/AbstractBpsEventHandler>

struct bps_event_t;

class VirtualKeyboardService : public QObject, public bb::AbstractBpsEventHandler
{
    Q_OBJECT
public:
    VirtualKeyboardService(QObject *parent = 0);
    ~VirtualKeyboardService();

    void requestEvents();
    virtual void event(bps_event_t *event);

    Q_INVOKABLE int height() const;
    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();
Q_SIGNALS:
    void keyboardVisible();
    void keyboardHidden();
};

QML_DECLARE_TYPE(VirtualKeyboardService)
