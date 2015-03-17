#include "VirtualKeyboardService.h"
#include "bps/virtualkeyboard.h"


VirtualKeyboardService::VirtualKeyboardService(QObject *parent)
    : QObject(parent)
{
    bps_initialize();
    subscribe(virtualkeyboard_get_domain());
    virtualkeyboard_request_events(0);
}

VirtualKeyboardService::~VirtualKeyboardService()
{
    virtualkeyboard_stop_events(0);
}

int VirtualKeyboardService::height() const
{
    int height = 0;
    virtualkeyboard_get_height(&height);
    return height;
}

void VirtualKeyboardService::show()
{
    virtualkeyboard_show();
}

void VirtualKeyboardService::hide()
{
    virtualkeyboard_hide();
}

void VirtualKeyboardService::event(bps_event_t *event)
{
    if (event == NULL)
        return;
    int domain = bps_event_get_domain(event);
    if (domain != virtualkeyboard_get_domain())
        return;

    uint16_t code = bps_event_get_code(event);
    switch (code)
    {
    case VIRTUALKEYBOARD_EVENT_VISIBLE:
        Q_EMIT keyboardVisible();
        break;
    case VIRTUALKEYBOARD_EVENT_HIDDEN:
        Q_EMIT keyboardHidden();
        break;
    }
}
