#ifndef MCS_DBQUERIESEMPLOYEE_H
#define MCS_DBQUERIESEMPLOYEE_H

#define GET_EMPLOYEE_PROPERTIES_BY_USER_NAME_CLINIC_DB "SELECT * FROM public.employees WHERE public.employees.user_name = '%1'"
#define GET_EMPLOYEE_PROPERTIES_BY_ID_CLINIC_DB "SELECT * FROM public.employees WHERE public.employees.ud = %1"
#define GET_EMPLOYEE_LOGS_BY_USER_NAME_CLINIC_DB "select * from logs where employee_id = (select id from employees where user_name = '%1')"
#define GET_EMPLOYEE_LOGS_BY_EMPLOYEE_ID_CLINIC_DB "select * from logs where employee_id = %1"

#endif // MCS_DBQUERIESEMPLOYEE_H
