#include "PhoneNumberInput.h"

#include <bb/cascades/Application>
#include <bb/cascades/Container>
#include <bb/cascades/DropDown>
#include <bb/cascades/StackLayout>
#include <bb/cascades/QmlDocument>

#include <bb/data/XmlDataAccess>

using namespace bb::cascades;
using namespace bb::data;

PhoneNumberInput::PhoneNumberInput()
{
    Container* container = Container::create();
    m_countryName = TextField::create();
    m_countryName->setHintText("Choose a country");
    m_countryName->setClearButtonVisible(false);
    m_countryName->setInputMode(TextFieldInputMode::Custom);
    m_countryName->input()->setFlags(TextInputFlag::VirtualKeyboardOff);
    container->add(m_countryName);

    Container* panel = Container::create();
    panel->setLayout(StackLayout::create().orientation(LayoutOrientation::LeftToRight));
    panel->setTopMargin(50);

    m_countryCode = TextField::create();
    m_countryCode->setInputMode(TextFieldInputMode::PhoneNumber);
    m_countryCode->setMaxWidth(150);
    connect(m_countryCode, SIGNAL(textChanging(QString)), this, SLOT(onContryCodeChanging(QString)));
    m_countryCode->setText("+");
    m_countryCode->setHintText("");
    panel->add(m_countryCode);


    m_phoneNumber = TextField::create();
    m_phoneNumber->setInputMode(TextFieldInputMode::PhoneNumber);
    m_phoneNumber->setHintText("");
    panel->add(m_phoneNumber);

    container->add(panel);

    setRoot(container);

    QObject::connect(m_countryName, SIGNAL(touch(bb::cascades::TouchEvent *)), this, SLOT(onCountryFocusChanged(bb::cascades::TouchEvent *)));

    m_dataModel = new GroupDataModel(this);
    m_dataModel->setSortingKeys(QStringList() << "name");
    m_dataModel->setGrouping(ItemGrouping::ByFirstChar);

    XmlDataAccess xda;
    QVariant list = xda.load(QDir::currentPath() +
                             "/app/native/assets/dm_countries.xml",
                             "/countries/country");
    m_countriesList = list.value<QVariantList>();
    m_dataModel->insertList(m_countriesList);
    emit dataModelChanged();
}

PhoneNumberInput::~PhoneNumberInput()
{
}

void PhoneNumberInput::onContryCodeChanging(const QString& text)
{
    if (!text.isEmpty() && !text.startsWith("+"))
    {
        m_countryCode->setText("+" + text);
    }
    else
    {
        QString s = text;
        int code = s.remove('+').toInt();
        QString country = "";
        for (int i = m_countriesList.size() - 1 ; i >= 0; i--)
        {
            QVariantMap item = m_countriesList[i].toMap();
            if (item["code"].toInt() == code)
            {
                country = item["name"].toString();
                break;
            }
        }
        m_countryName->setText(country);
    }
}

GroupDataModel* PhoneNumberInput::dataModel() const
{
    return m_dataModel;
}

void PhoneNumberInput::setFilter(const QString& filter)
{
    m_dataModel->clear();
    if (filter.isEmpty())
        m_dataModel->insertList(m_countriesList);
    else
    {
        QVariantList filteredList;
        for(QVariantList::const_iterator it = m_countriesList.begin(); it != m_countriesList.end(); ++it)
        {
            QMap<QString, QVariant> country = it->toMap();
            if(country["name"].toString().startsWith(filter, Qt::CaseInsensitive))
                filteredList.append(*it);
        };
        m_dataModel->insertList(filteredList);
    }
}

void PhoneNumberInput::setCountry(const QString& name, const QString& code)
{
    m_countryCode->setText("+" + code);
    m_countryName->setText(name);

    NavigationPane* navigationPane = static_cast<NavigationPane*>(Application::instance()->scene());
    if (navigationPane)
       navigationPane->pop();

    //m_countryName->focus
}

QString PhoneNumberInput::phone() const
{
    return m_countryCode->text() + m_phoneNumber->text();
}

void PhoneNumberInput::onCountryFocusChanged(bb::cascades::TouchEvent* event)
{
    if (event->touchType() == TouchType::Down)
    {
        NavigationPane* navigationPane = static_cast<NavigationPane*>(Application::instance()->scene());
        if (navigationPane)
        {
            QmlDocument* qml = QmlDocument::create("asset:///ui/pages/CountrySelect.qml");
            Page* page = qml->createRootObject<Page>();

            qml->setContextProperty("_phoneNumberInput", this);

            navigationPane->push(page);
        }
    }
}
