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
    m_CountryName = TextField::create();
    m_CountryName->setHintText("Choose a country");
    m_CountryName->setClearButtonVisible(false);
    m_CountryName->setInputMode(TextFieldInputMode::Custom);
    m_CountryName->input()->setFlags(TextInputFlag::VirtualKeyboardOff);
    container->add(m_CountryName);

    Container* panel = Container::create();
    panel->setLayout(StackLayout::create().orientation(LayoutOrientation::LeftToRight));
    panel->setTopMargin(50);

    m_CountryCode = TextField::create();
    m_CountryCode->setInputMode(TextFieldInputMode::PhoneNumber);
    m_CountryCode->setMaxWidth(150);
    m_CountryCode->setText("+7");
    panel->add(m_CountryCode);

    m_PhoneNumber = TextField::create();
    m_PhoneNumber->setInputMode(TextFieldInputMode::PhoneNumber);
    panel->add(m_PhoneNumber);

    container->add(panel);

    setRoot(container);

    QObject::connect(m_CountryName, SIGNAL(focusedChanged(bool)), this, SLOT(onCountryFocusChanged(bool)));

    m_DataModel = new GroupDataModel(this);
    m_DataModel->setSortingKeys(QStringList() << "name");
    m_DataModel->setGrouping(ItemGrouping::ByFirstChar);

    XmlDataAccess xda;
    QVariant list = xda.load(QDir::currentPath() +
                             "/app/native/assets/dm_countries.xml",
                             "/countries/country");
    m_CountriesList = list.value<QVariantList>();
    m_DataModel->insertList(m_CountriesList);
    emit dataModelChanged();
}

PhoneNumberInput::~PhoneNumberInput()
{
}

GroupDataModel* PhoneNumberInput::dataModel() const
{
    return m_DataModel;
}

void PhoneNumberInput::setFilter(const QString& filter)
{
    m_DataModel->clear();
    if (filter.isEmpty())
        m_DataModel->insertList(m_CountriesList);
    else
    {
        QVariantList filteredList;
        for(QVariantList::const_iterator it = m_CountriesList.begin(); it != m_CountriesList.end(); ++it)
        {
            QMap<QString, QVariant> country = it->toMap();
            if(country["name"].toString().startsWith(filter, Qt::CaseInsensitive))
                filteredList.append(*it);
        };
        m_DataModel->insertList(filteredList);
    }
}

void PhoneNumberInput::setCountry(const QString& name, const QString& code)
{
    m_CountryName->setText(name);
    m_CountryCode->setText("+" + code);

    NavigationPane* navigationPane = static_cast<NavigationPane*>(Application::instance()->scene());
    if (navigationPane)
       navigationPane->pop();
}

QString PhoneNumberInput::phone() const
{
    return m_CountryCode->text() + m_PhoneNumber->text();
}

void PhoneNumberInput::onCountryFocusChanged(bool focused)
{
    if (focused)
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
