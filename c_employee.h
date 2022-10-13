#ifndef C_EMPLOYEE_H
#define C_EMPLOYEE_H

#include "m_employee.h"

#include <QObject>

class c_employee : public m_employee
{
    Q_OBJECT
public:
    c_employee(QObject *parent = nullptr);
    c_employee(quint32 id, QString name, QString last_name, QObject *parent = nullptr);

signals:

};

#endif // C_EMPLOYEE_H
