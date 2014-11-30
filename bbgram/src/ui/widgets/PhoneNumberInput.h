#include <bb/cascades/CustomControl>
#include <bb/cascades/GroupDataModel>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/TextField>

class PhoneNumberInput: public bb::cascades::CustomControl
{
    Q_OBJECT

    Q_PROPERTY(bb::cascades::GroupDataModel* dataModel READ dataModel NOTIFY dataModelChanged)
    Q_PROPERTY(QString phone READ phone)
public:
    PhoneNumberInput();
    virtual ~PhoneNumberInput();

    Q_INVOKABLE
    void setFilter(const QString& filter);
    Q_INVOKABLE
    void setCountry(const QString& name, const QString& code);

    QString phone() const;

    bb::cascades::GroupDataModel* dataModel() const;
signals:
    void dataModelChanged();
protected:
    bb::cascades::TextField* m_countryName;
    bb::cascades::TextField* m_countryCode;
    bb::cascades::TextField* m_phoneNumber;

    bb::cascades::GroupDataModel* m_dataModel;
    QVariantList    m_countriesList;
protected slots:
    void onCountryFocusChanged(bool focused);
};
