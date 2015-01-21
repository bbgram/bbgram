#include <bb/cascades/CustomControl>
#include <bb/cascades/Container>

#include "../../model/Message.h"

class DocumentViewer: public bb::cascades::CustomControl
{
    Q_OBJECT

    Q_PROPERTY(Message* message READ message WRITE setMessage)
public:
    DocumentViewer();
    virtual ~DocumentViewer();

    Message* message() const;
    void setMessage(Message *message);
protected:
    Message* m_message;
    bb::cascades::Container* m_root;
};
