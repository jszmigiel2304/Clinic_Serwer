#ifndef MCS_DBQUERIESEMPLOYEE_H
#define MCS_DBQUERIESEMPLOYEE_H

#define GET_EMPLOYEE_PROPERTIES_BY_USER_NAME_CLINIC_DB "SELECT m.*, p.id AS position_id, p.position AS position_name, p.male_form AS position_male_form, p.female_form AS position_female_form, e.id AS supervisor_id, e.name AS supervisor_name, e.last_name AS supervisor_last_name FROM employees m LEFT JOIN employees e ON m.supervisor_id = e.id LEFT JOIN employee_positions p ON m.position_id = p.id WHERE m.user_name = '%1';"
#define GET_EMPLOYEE_PROPERTIES_BY_ID_CLINIC_DB "SELECT m.*, p.id AS position_id, p.position AS position_name, p.male_form AS position_male_form, p.female_form AS position_female_form, e.id AS supervisor_id, e.name AS supervisor_name, e.last_name AS supervisor_last_name FROM employees m LEFT JOIN employees e ON m.supervisor_id = e.id LEFT JOIN employee_positions p ON m.position_id = p.id WHERE m.id = %1;"
#define GET_EMPLOYEE_LOGS_BY_USER_NAME_CLINIC_DB "select * from logs where employee_id = (select id from employees where user_name = '%1')"
#define GET_EMPLOYEE_LOGS_BY_EMPLOYEE_ID_CLINIC_DB "select * from logs where employee_id = %1"

#endif // MCS_DBQUERIESEMPLOYEE_H
