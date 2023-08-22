#if defined _admin_report_included
    #endinput
#endif
#define _admin_report_included

/*
	check libs
*/

#if !defined _FOREACH_LOCAL_VERSION
    #error "Please included <foreach.inc> before include <admin_report.inc>"
#endif

#if !defined _mdialog_included
    #error "Please included <mdialog.inc> before include <admin_report.inc>" 
#endif

#if !defined _INC_SSCANF
    #warning "Please include <sscanf2.inc> to correct work."
#endif

#if !defined CMD
    #warning "Please include <Pawn.CMD.inc> to correct work."
#endif

/*
	definitons
*/

#if !defined MAX_ADMIN_REPORT_COUNT 
    #define MAX_ADMIN_REPORT_COUNT (25)
#endif

#if !defined MAX_ADMIN_REPORT_MESSAGE_LENGTH
    #define MAX_ADMIN_REPORT_MESSAGE_LENGTH (60)
#endif

#if !defined MAX_ADMIN_REPORT_MESSAGE_DELAY
    #define MAX_ADMIN_REPORT_MESSAGE_DELAY (60)
#endif

#if !defined MAX_ADMIN_REPORT_LISTITEM
	#define MAX_ADMIN_REPORT_LISTITEM (MAX_ADMIN_REPORT_COUNT) 
#endif

#if !defined MIN_ADMIN_REPORT_LEVEL
    #define MIN_ADMIN_REPORT_LEVEL (0)
#endif

#if !defined ADMIN_REPORT_HEX_COLOR
	#define ADMIN_REPORT_HEX_COLOR 0xFFFFFFFF 	
#endif

/*  
    enums
*/

static enum E_ADMIN_REPORT_STRUCT {
    AR_ID,
    AR_MESSAGE[MAX_ADMIN_REPORT_MESSAGE_LENGTH + 1],
    AR_UNIX_TIME
};

static enum E_PLAYER_ADMIN_REPORT_STRUCT {
    PAR_NAME[MAX_PLAYER_NAME],
    PAR_LEVEL,
    PAR_DELAY,
    PAR_LISTITEM[MAX_ADMIN_REPORT_LISTITEM]
};

static 
    g_admin_report[MAX_ADMIN_REPORT_COUNT][E_ADMIN_REPORT_STRUCT],
    g_player_admin_report[MAX_PLAYERS][E_PLAYER_ADMIN_REPORT_STRUCT];

static const 
    NULL_g_admin_report[E_ADMIN_REPORT_STRUCT] = {
    INVALID_PLAYER_ID,
    EOS,
    -1
},
    NULL_g_player_admin_report[E_PLAYER_ADMIN_REPORT_STRUCT] = {
    EOS,
    MIN_ADMIN_REPORT_LEVEL,
    -1,
    0
};

/*  
    vars
*/

new 
    Iterator: g_admin_report_iterator<MAX_ADMIN_REPORT_COUNT>,
    Iterator: g_admins_iterator<MAX_PLAYERS>,
    pVar__AdminReport_IDX[] = !"pVar__AdminReport_IDX",
    g_player_admin_report_listitem[MAX_PLAYERS][MAX_ADMIN_REPORT_LISTITEM] = { { 0, ...}, ...};

/*  
    functions
*/

stock SetPlayerAdminReportLevel(const playerid, const value)
{
    if (!value) {
        if (Iter_Contains(g_admins_iterator, playerid)) {
            Iter_Remove(g_admins_iterator, playerid);    
        }
    } else {
        if (Iter_Contains(g_admins_iterator, playerid)) {
            Iter_Add(g_admins_iterator, playerid);    
        }
    }

    SetPlayerAdminReportLevel(playerid, value);
    
    return 1;
}

stock SetPlayerAdminReportDataStr(const playerid, const E_PLAYER_ADMIN_REPORT_STRUCT: data, const buffer[])
{
    format(
            g_player_admin_report[playerid][data],
            MAX_PLAYER_NAME,
            "%s",
            buffer
    );
    return 1;
}

stock GetPlayerAdminReportDataStr(const playerid, const E_PLAYER_ADMIN_REPORT_STRUCT: data, buffer[])
{
    format(
            buffer,
            MAX_PLAYER_NAME,
            "%s",
            g_player_admin_report[playerid][data]
    );
    return 1;
}

stock SetPlayerAdminReportDataInt(const playerid, const E_PLAYER_ADMIN_REPORT_STRUCT: data, const value)
{
    g_player_admin_report[playerid][data] = value;
}

stock GetPlayerAdminReportDataInt(const playerid, const E_PLAYER_ADMIN_REPORT_STRUCT: data)
{
    return g_player_admin_report[playerid][data];
}

stock ClearPlayerAdminReportData(const playerid)
{
    g_player_admin_report[playerid] = NULL_g_player_admin_report;
}

stock SetAdminReportDataStr(const idx, const E_ADMIN_REPORT_STRUCT: data, const buffer[])
{
    format(
            g_admin_report[idx][data], 
            MAX_ADMIN_REPORT_MESSAGE_LENGTH, 
            "%s", 
            buffer
    );
    return 1;
}

stock GetAdminReportDataStr(const idx, const E_ADMIN_REPORT_STRUCT: data, buffer[])
{
    format(
            buffer, 
            MAX_ADMIN_REPORT_MESSAGE_LENGTH, 
            "%s", 
            g_admin_report[idx][data]
    );
    return 1;
}

stock GetAdminReportDataInt(const idx, const E_ADMIN_REPORT_STRUCT: data)
{
    return g_admin_report[idx][E_ADMIN_REPORT_STRUCT: data];
}

stock SetAdminReportDataInt(const idx, const E_ADMIN_REPORT_STRUCT: data, const value)
{
    g_admin_report[idx][E_ADMIN_REPORT_STRUCT: data] = value;
}

stock SetPlayerAdminReportListitem(const playerid, const idx, const value)
{
    g_player_admin_report_listitem[playerid][idx] = value;
}

stock IsAdminReportEmpty(const idx)
{
    return (isnull(g_admin_report[idx][AR_MESSAGE])) ? (0) : (1);
}

stock SendAdminReportMessage(const color, const message[])
{
    foreach (new idx : g_admins_iterator) {
        SendClientMessage(idx, color, message);
    }
    
    return 1;
}

stock AdminReportCreate(const idx, &playerid, &unix_time, const message[])
{  
    SetAdminReportDataInt(idx, AR_ID, playerid);
    SetAdminReportDataInt(idx, AR_UNIX_TIME, unix_time);
    SetAdminReportDataStr(idx, AR_MESSAGE, message);

    Iter_Add(g_admin_report_iterator, idx);

    return 1;
}

stock AdminReportDelete(const idx)
{
    g_admin_report[idx][AR_ID] = NULL_g_admin_report[AR_ID];
    g_admin_report[idx][AR_UNIX_TIME] = NULL_g_admin_report[AR_UNIX_TIME];
    g_admin_report[idx][AR_MESSAGE][0] = NULL_g_admin_report[AR_MESSAGE];

    Iter_Remove(g_admin_report_iterator, idx);
    
    return 1;
}   

stock AdminReportAnswer(const playerid, const idx, const message[])
{
    new 
        string[32 + (- 2 + MAX_PLAYER_NAME) + (- 2 + 3) + (- 2 + MAX_ADMIN_REPORT_MESSAGE_LENGTH) + 1],
        player_name[MAX_PLAYER_NAME + 1],
        id = GetAdminReportDataInt(idx, AR_ID);

    GetPlayerAdminReportDataStr(playerid, PAR_NAME, player_name);

    format(
            string, 
            sizeof(string),
            "%s[%d] ответил на Ваш репорт: %s",
            player_name,
            playerid,
            message
    );
    SendClientMessage(id, ADMIN_REPORT_HEX_COLOR, string);
    
    return 1;
}

/*  
	hook OnPlayerConnect
*/

public OnPlayerConnect(playerid)
{
    new player_name[MAX_PLAYER_NAME + 1];
    GetPlayerName(playerid, player_name, MAX_PLAYER_NAME);

    SetPlayerAdminReportDataStr(playerid, PAR_NAME, player_name);
    
	#if defined ar_OnPlayerConnect
		return ar_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect ar_OnPlayerConnect
#if	defined ar_OnPlayerConnect
	forward ar_OnPlayerConnect(playerid);
#endif

/*  
    hook OnPlayerDisconnect
*/

public OnPlayerDisconnect(playerid, reason)
{
    if (Iter_Count(g_admin_report_iterator)) {
        new id;

        foreach (new idx : g_admin_report_iterator) {
            id = GetAdminReportDataInt(idx, AR_ID);

            if (id != playerid) {
                continue;
            }
            
            AdminReportDelete(idx);
        }
    }

    ClearPlayerAdminReportData(playerid);

    SetPlayerAdminReportLevel(playerid, MIN_ADMIN_REPORT_LEVEL);

    #if defined ar_OnPlayerDisconnect
		return ar_OnPlayerDisconnect(playerid, reason);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerDisconnect
	#undef OnPlayerDisconnect
#else
	#define _ALS_OnPlayerDisconnect
#endif

#define OnPlayerDisconnect ar_OnPlayerDisconnect
#if defined ar_OnPlayerDisconnect
	forward ar_OnPlayerDisconnect(playerid, reason);
#endif

/*
    dialogs
*/

DialogCreate:D_ADMIN_REPORT_LIST(playerid) 
{
    new 
        string[(18 + (- 2 + MAX_PLAYER_NAME) + (- 2 + 3) + (- 2 + 11)) * MAX_ADMIN_REPORT_COUNT + 1] = "Игрок\t\tВремя\n",
        player_name[MAX_PLAYER_NAME + 1],
        id,
        time,
        slot = 0;

    GetPlayerAdminReportDataStr(playerid, PAR_NAME, player_name);

    foreach (new idx : g_admin_report_iterator) {
        if (!IsAdminReportEmpty(idx)) {
            continue;
        }

		SetPlayerAdminReportDataInt(playerid, PAR_LISTITEM, slot);

		id = GetAdminReportDataInt(idx, AR_ID);
		time = GetAdminReportDataInt(idx, AR_UNIX_TIME);
		
        format(
                string, 
                sizeof string, 
                "%s%s[%d]\t\t%d мин\n",
                string,
                player_name,
                id,
                (gettime() - time) / 60
        );
    }

    Dialog_Open(
            playerid, 
            Dialog:D_ADMIN_REPORT_LIST, 
            DIALOG_STYLE_TABLIST_HEADERS,
            !"{FFFFFF}Список репортов",
            string,
            !"Выбрать", !"Закрыть"
    );
}

DialogResponse:D_ADMIN_REPORT_LIST(playerid, response, listitem, inputtext[]) 
{
    if (!response) {
        return 0;
    }

    new slot = g_player_admin_report[playerid][PAR_LISTITEM][listitem];

    if (!IsAdminReportEmpty(slot)) {
        return SendClientMessage(playerid, ADMIN_REPORT_HEX_COLOR, !"Данный репорт был удалён из списка.");
    }

    SetPVarInt(playerid, pVar__AdminReport_IDX, slot);

    return Dialog_Show(playerid, Dialog:D_ADMIN_REPORT_INFO);
}

DialogCreate:D_ADMIN_REPORT_INFO(playerid) 
{
    new
        string[80 + (- 2 + MAX_PLAYER_NAME) + (- 2 + 11) + (- 2 + MAX_ADMIN_REPORT_MESSAGE_LENGTH) + 1],
        player_name[MAX_PLAYER_NAME + 1],
        idx = GetPVarInt(playerid, pVar__AdminReport_IDX),
        id = GetAdminReportDataInt(idx, AR_ID),
		time = GetAdminReportDataInt(idx, AR_UNIX_TIME),
		message[MAX_ADMIN_REPORT_MESSAGE_LENGTH + 1];
		
	GetAdminReportDataStr(idx, AR_MESSAGE, message);
	GetPlayerAdminReportDataStr(id, PAR_NAME, player_name);

    format(
            string, 
            sizeof string, 
            "{FFFFF}Репорт от: %s[%d]\n\
            Время: %d мин.\n\
            Текст: %s",
            player_name,
            id,
            (gettime() - time) / 60,
            message
    );

    Dialog_Open(
            playerid, 
            Dialog:D_ADMIN_REPORT_INFO,  
            DIALOG_STYLE_MSGBOX,
            !"{FFFFFF}Информация о репорте",
            string,
            !"Далее", !"Назад"
    );
}

DialogInterrupt:D_ADMIN_REPORT_INFO(playerid)
{
    DeletePVar(playerid, pVar__AdminReport_IDX);
    return 1;
}

DialogResponse:D_ADMIN_REPORT_INFO(playerid, response, listitem, inputtext[]) 
{
    if (!response) {
        DeletePVar(playerid, pVar__AdminReport_IDX);
        return Dialog_Show(playerid, Dialog:D_ADMIN_REPORT_LIST);
    }

    return Dialog_Show(playerid, Dialog:D_ADMIN_REPORT_MENU);
}

DialogCreate:D_ADMIN_REPORT_MENU(playerid) 
{
    Dialog_Open(
            playerid, 
            Dialog:D_ADMIN_REPORT_MENU,  
            DIALOG_STYLE_LIST,
            " ",
            "Ответить\n\
            Удалить",
            !"Далее", !"Закрыть"
    );
}

DialogInterrupt:D_ADMIN_REPORT_MENU(playerid)
{
    DeletePVar(playerid, pVar__AdminReport_IDX);
    return 1;
}

DialogResponse:D_ADMIN_REPORT_MENU(playerid, response, listitem, inputtext[]) 
{
    if (!response) {
        DeletePVar(playerid, pVar__AdminReport_IDX);
        return 0;
    }   

    new idx = GetPVarInt(playerid, pVar__AdminReport_IDX);
    
    DeletePVar(playerid, pVar__AdminReport_IDX);
    
    if (!IsAdminReportEmpty(idx)) {
        return SendClientMessage(playerid, -1, !"Выбранный репорт был удалён.");
    }

    switch (listitem) {
        case 0: {
            AdminReportAnswer(playerid, idx, inputtext);
            AdminReportDelete(idx);
            return 1;
            
        }
        case 1: {
            AdminReportDelete(idx);
            return 1;
        }
    }

    return 1;
}

/*  
    commands
*/

cmd:send_report(playerid, params[])
{
    new free_idx = Iter_Free(g_admin_report_iterator);

    if (free_idx == ITER_NONE) {
        return SendClientMessage(playerid, ADMIN_REPORT_HEX_COLOR, !"Cписок репортов переполнен.");
    }

    if (IsAdminReportEmpty(free_idx)) {
        return 0;
    }

    if (GetPlayerAdminReportDataInt(playerid, PAR_DELAY) >= gettime()) {
        return SendClientMessage(playerid, ADMIN_REPORT_HEX_COLOR, !"Отправлять репорт можно раз в 1 минуту.");
    }

    if (isnull(params)) {
        return SendClientMessage(playerid, ADMIN_REPORT_HEX_COLOR, !"Используйте: /send_report [сообщение]");
    }

    new 
        string[27 + (- 2 + MAX_PLAYER_NAME) + (- 2 + 3) + (- 2 + MAX_ADMIN_REPORT_MESSAGE_LENGTH) + 1],
        player_name[MAX_PLAYER_NAME + 1],
        unix_time = gettime();

    GetPlayerAdminReportDataStr(playerid, PAR_NAME, player_name);

    format(
            string, 
            sizeof string, 
            "Репорт от игрока %s[%d]: %s",
            player_name,
            playerid,
            params
    );
    SendAdminReportMessage(ADMIN_REPORT_HEX_COLOR, string);

    AdminReportCreate(free_idx, playerid, unix_time, params);
    SetPlayerAdminReportDataInt(playerid, PAR_DELAY, unix_time + MAX_ADMIN_REPORT_MESSAGE_DELAY);

    return SendClientMessage(playerid, ADMIN_REPORT_HEX_COLOR, !"Ваш репорт успешно добавлен в очередь.");
}

cmd:show_reports(playerid)
{
    if (GetPlayerAdminReportDataInt(playerid, PAR_LEVEL) == MIN_ADMIN_REPORT_LEVEL) {
        return 0;
    }

    if (!Iter_Count(g_admin_report_iterator)) {
        return SendClientMessage(playerid, ADMIN_REPORT_HEX_COLOR, !"Список репортов пуст.");
    }

    return Dialog_Show(playerid, Dialog:D_ADMIN_REPORT_LIST);
}