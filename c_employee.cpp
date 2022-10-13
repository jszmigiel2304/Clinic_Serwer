#include "c_employee.h"

c_employee::c_employee(QObject *parent)
    : m_employee(parent)
{

}

c_employee::c_employee(quint32 id, QString name, QString last_name, QObject *parent)
    : m_employee(id, name, last_name, parent)
{

}



