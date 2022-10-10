#ifndef MCS_DBQUERIESSESSIONCONTROLLER_H
#define MCS_DBQUERIESSESSIONCONTROLLER_H

#define ADD_NEW_SESSION_AUTH_DB "INSERT INTO public.sessions(id, id_user, begin_time, end_time, ip_address, activity, state) VALUES ('%1', %2, '%3', NULL, '%4', NULL, 'DEFINED_NOT_STARTED') RETURNING *"

#define UPDATE_SESSION_ACTIVITY_AUTH_DB "UPDATE public.sessions SET activity = '%1' WHERE id = '%2'"
#define UPDATE_SESSION_STATE_AUTH_DB "UPDATE public.sessions SET state = '%2' WHERE id = '%1'"
#define UPDATE_SESSION_CLOSE_TIME_AUTH_DB "UPDATE public.sessions SET end_time = '%2' WHERE id = '%1'"


#endif // MCS_DBQUERIESSESSIONCONTROLLER_H
