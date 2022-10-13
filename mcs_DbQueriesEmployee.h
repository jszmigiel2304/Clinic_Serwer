#ifndef MCS_DBQUERIESEMPLOYEE_H
#define MCS_DBQUERIESEMPLOYEE_H

#define GET_USER_AUTH_DB "SELECT * FROM public.users WHERE public.users.id = %1 AND public.users.name = '%2' AND public.users.password = '\\x%3'"
#define GET_USER_BY_NAME_PASSWORD_AUTH_DB "SELECT * FROM public.users WHERE public.users.name = '%1' AND public.users.password = '\\x%2'"

#endif // MCS_DBQUERIESEMPLOYEE_H
