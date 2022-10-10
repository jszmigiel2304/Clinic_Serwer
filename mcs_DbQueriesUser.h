#ifndef MCS_DBQUERIESUSER_H
#define MCS_DBQUERIESUSER_H

#define GET_USER_AUTH_DB "SELECT * FROM public.users WHERE public.users.id = %1 AND public.users.name = '%2' AND public.users.password = '\\x%3'"
#define GET_USER_ID_AUTH_DB "SELECT public.users.id FROM public.users WHERE public.users.name = '%1' AND public.users.password = '\\x%2'"

#define ADD_LOGGING_LOG_AUTH_DB "INSERT INTO public.logs(id_user, log_time, ip_address, activity) VALUES(%1, '%2', '%3', %4)"
#define UPDATE_USER_IS_LOGGED_AUTH_DB "UPDATE public.users SET logged = %2 WHERE id = %1"

#define GET_USERS_UNFINISHED_SESSIONS_AUTH_DB "SELECT * FROM public.sessions WHERE public.sessions.id_user = %1 AND public.sessions.state != 'CLOSED_SAVED' ORDER BY public.sessions.begin_time DESC "


#endif // MCS_DBQUERIESUSER_H
