#include <bb/cascades/CustomControl>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/TextField>

class PhoneNumberInput: public bb::cascades::CustomControl
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::GroupDataModel* dataModel READ dataModel NOTIFY dataModelChanged)
public:
    PhoneNumberInput();
    virtual ~PhoneNumberInput();

    Q_INVOKABLE
    void setFilter(const QString& filter);
    Q_INVOKABLE
    void setCountry(const QString& name, const QString& code);

    bb::cascades::GroupDataModel* dataModel() const;
signals:
    void dataModelChanged();
protected:
    bb::cascades::TextField* m_CountryName;
    bb::cascades::TextField* m_CountryCode;
    bb::cascades::TextField* m_PhoneNumber;

    bb::cascades::GroupDataModel* m_DataModel;
    QVariantList    m_CountriesList;
protected slots:
    void onCountryFocusChanged(bool focused);
};
