#ifndef C_EMPLOYEE_H
#define C_EMPLOYEE_H

#include "m_employee.h"
#include "mcs_DbQueriesEmployee.h"

#include <QObject>

class c_employee : public m_employee
{
    Q_OBJECT
public:
    c_employee(QObject *parent = nullptr);
    c_employee(quint32 id, QString name, QString last_name, QObject *parent = nullptr);

    QString getSelectEmployeePropertiesByUserNameQuery(QString name) const;
    QString getSelectEmployeePropertiesByIdQuery(quint32 id) const;
    QString getSelectEmployeeLogsByUserNameQuery(QString name) const;
    QString getSelectEmployeeLogsByEmployeeIdQuery(quint32 id) const;

signals:

};

#endif // C_EMPLOYEE_H
