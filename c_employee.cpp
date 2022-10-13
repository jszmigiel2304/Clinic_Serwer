#include "c_employee.h"

c_employee::c_employee(QObject *parent)
    : m_employee(parent)
{

}

c_employee::c_employee(quint32 id, QString name, QString last_name, QObject *parent)
    : m_employee(id, name, last_name, parent)
{

}

QString c_employee::getSelectEmployeePropertiesByUserNameQuery(QString name) const
{
    QString query = QString(GET_EMPLOYEE_PROPERTIES_BY_USER_NAME_CLINIC_DB).arg(name);
    return query;
}

QString c_employee::getSelectEmployeePropertiesByIdQuery(quint32 id) const
{
    QString query = QString(GET_EMPLOYEE_PROPERTIES_BY_ID_CLINIC_DB).arg(id);
    return query;
}

QString c_employee::getSelectEmployeeLogsByUserNameQuery(quint32 id) const
{
    QString query = QString(GET_EMPLOYEE_LOGS_BY_USER_NAME_CLINIC_DB).arg(id);
    return query;
}

QString c_employee::getSelectEmployeeLogsByEmployeeIdQuery(quint32 id) const
{
    QString query = QString(GET_EMPLOYEE_LOGS_BY_EMPLOYEE_ID_CLINIC_DB).arg(id);
    return query;
}



