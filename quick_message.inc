#if defined _quick_message_included
    #endinput
#endif
#define _quick_message_included


/*
	check libs
*/

#if !defined mysql_included || !defined cache_get_value_name
    #error "Please included <a_mysql.inc> version R40+ before include <quick_message.inc>" 
#endif

#if !defined _FOREACH_LOCAL_VERSION
    #error "Please included <foreach.inc> before include <quick_message.inc>"
#endif

#if !defined CMD
    #error "Please include <Pawn.CMD.inc> or <dc_cmd.inc> to correct work."
#endif

/*
	definitons
*/

#if !defined QM_TABLE
    #define QM_TABLE "quick_message"
#endif

#if !defined QM_TABLE_ACCOUNTS
    #define QM_TABLE_ACCOUNTS "users"
#endif

#if !defined QM_STATIC_ENUM
    #if defined __PawnBuild
		#define QM_STATIC_ENUM static
	#else
		#define QM_STATIC_ENUM
	#endif
#endif

#if !defined HEX_COLOR_QM_ERROR 
    #define HEX_COLOR_QM_ERROR 0xE05A5AFF 
#endif 
  
#if !defined HEX_COLOR_QM_SUCCESS 
    #define HEX_COLOR_QM_SUCCESS 0x2ECC71FF
#endif

#if !defined MAX_PLAYER_QM_COUNT
    #define MAX_PLAYER_QM_COUNT 20
#endif

#if !defined MAX_PLAYER_QM_TITLE_LENGTH
    #define MAX_PLAYER_QM_TITLE_LENGTH 32
#endif

#if !defined MIN_PLAYER_QM_TITLE_LENGTH
    #define MIN_PLAYER_QM_TITLE_LENGTH 4
#endif

#if !defined MAX_PLAYER_QM_LENGTH
    #define MAX_PLAYER_QM_LENGTH 128
#endif

#if !defined MIN_PLAYER_QM_LENGTH
    #define MIN_PLAYER_QM_LENGTH 4
#endif

#if !defined MAX_QM_TAG_NAME_LENGTH
    #define MAX_QM_TAG_NAME_LENGTH 5
#endif

#if !defined MAX_QM_COMMAND_NAME_LENGTH
    #define MAX_QM_COMMAND_NAME_LENGTH 4
#endif

/*
	enums
*/

QM_STATIC_ENUM enum {
    DIALOG_QM_MAIN_MENU = 7500,
    DIALOG_QM_EDIT_MENU,
    DIALOG_QM_TITLE,
    DIALOG_QM_CREATE,
    DIALOG_QM_DESCRIPTION,
    DIALOG_QM_EDIT,
    DIALOG_QM_DELETE,
    DIALOG_QM_START,
}

QM_STATIC_ENUM enum {
    QM_EDIT_MENU_TYPE_DESCRIPTION,
    QM_EDIT_MENU_TYPE_EDIT,
    QM_EDIT_MENU_TYPE_DELETE,
    QM_EDIT_MENU_TYPE_START
}

QM_STATIC_ENUM enum {
    QM_TAG_NAME
}

QM_STATIC_ENUM enum E_QM_TAG_STRUCT {
    E_QM_TAG_NAME[MAX_QM_TAG_NAME_LENGTH + 1]
};

QM_STATIC_ENUM enum E_QM_COMMAND_STRUCT {
    E_QM_COMMAND_NAME[MAX_QM_COMMAND_NAME_LENGTH + 1]
};

QM_STATIC_ENUM enum E_PLAYER_QM_MAIN_STRUCT {
    E_PLAYER_QM_MAIN_ID,
    E_PLAYER_QM_MAIN_NAME[MAX_PLAYER_NAME + 1]
};

QM_STATIC_ENUM enum E_PLAYER_QM_STRUCT {
    E_PLAYER_QM_MESSAGE[MAX_PLAYER_QM_LENGTH + 1],
    E_PLAYER_QM_TITLE[MAX_PLAYER_QM_TITLE_LENGTH + 1],
    E_PLAYER_QM_UNIX_CREATE,
    E_PLAYER_QM_UNIX_LAST_EDIT
};

/*
	vars
*/

static
    g_player_qm_main_data[MAX_PLAYERS][E_PLAYER_QM_MAIN_STRUCT],
    g_player_qm_info[MAX_PLAYERS][MAX_PLAYER_QM_COUNT][E_PLAYER_QM_STRUCT],
    pVar__QuickMessageIndex[] = !"pVar__QuickMessageIndex",
    Iterator: iPlayerQuickMessage[MAX_PLAYERS]<MAX_PLAYER_QM_COUNT>,
    MySQL: g_qm_sql_handle;

static const
    NULL_g_player_qm_main_data[E_PLAYER_QM_MAIN_STRUCT] = {
    0,
    EOS
},
    NULL_g_player_qm_info[E_PLAYER_QM_STRUCT] = {
    EOS,
    EOS,
    0,
    0
},
    g_qm_tag_info[][E_QM_TAG_STRUCT] = {
    {"@name"}
},
    g_qm_command_info[][E_QM_COMMAND_STRUCT] = {
    {"/me"},
    {"/do"},
    {"/try"}
};

/*
	hook OnGameModeInit
*/

public OnGameModeInit()
{
    QM_Init();

    #if defined QM_OnGameModeInit
        QM_OnGameModeInit();
	#endif
	return 1;
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit QM_OnGameModeInit
#if	defined	QM_OnGameModeInit
	forward QM_OnGameModeInit();
#endif

/*  
    hook OnPlayerConnect
*/

public OnPlayerConnect(playerid)
{
    QM_PlayerMainClear(playerid);
    QM_PlayerClear(playerid);
    
    GetPlayerName(playerid, g_player_qm_main_data[playerid][E_PLAYER_QM_MAIN_NAME], MAX_PLAYER_NAME);

    #if defined QM_OnPlayerConnect
        QM_OnPlayerConnect(playerid);
    #endif
    return 1;
}
#if defined _ALS_OnPlayerConnect
    #undef OnPlayerConnect
#else
    #define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect QM_OnPlayerConnect
#if defined QM_OnPlayerConnect
    forward QM_OnPlayerConnect(playerid);
#endif

/*
    hook OnDialogResponse
*/

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch (dialogid) {
        case DIALOG_QM_MAIN_MENU: {
            if (!response) {
                return 0;
            }

            if (!strcmp(inputtext, "> Добавить быстрое сообщение")) {
                return QM_ShowPlayerTitleDialog(playerid);
            }

            return QM_ShowPlayerEditMenuDialog(playerid, listitem);
        }
        case DIALOG_QM_EDIT_MENU: {
            if (!response) {
                DeletePVar(playerid, pVar__QuickMessageIndex);

                return QM_ShowPlayerMainMenuDialog(playerid);
            }

            return QM_ShowPlayerOperateEditMenu(playerid, listitem);
        }
        case DIALOG_QM_TITLE: {
            if (!response) {
                return QM_ShowPlayerMainMenuDialog(playerid);
            }
            
            new len = strlen(inputtext);

            if (!(MIN_PLAYER_QM_TITLE_LENGTH <= len <= MAX_PLAYER_QM_TITLE_LENGTH)) {
                return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, 
                    !"Максимальная длина заголовка должна быть в диапазоне: от "#MIN_PLAYER_QM_TITLE_LENGTH" до "#MAX_PLAYER_QM_TITLE_LENGTH".");
            }

            new index = Iter_Free(iPlayerQuickMessage[playerid]);
    
            if (index == ITER_NONE) {
                return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, !"Исчерпан доступный лимит быстрых сообщений.");
            }

            SetPVarInt(playerid, pVar__QuickMessageIndex, index);

            g_player_qm_info[playerid][index][E_PLAYER_QM_TITLE] = NULL_g_player_qm_info[E_PLAYER_QM_TITLE];

            strmid(g_player_qm_info[playerid][index][E_PLAYER_QM_TITLE], inputtext, 0, strlen(inputtext));

            return QM_ShowPlayerAddDialog(playerid);
        }
        case DIALOG_QM_CREATE: {
            if (!response) {
                return 0;
            }

            new len = strlen(inputtext);

            if (!(MIN_PLAYER_QM_LENGTH <= len <= MAX_PLAYER_QM_LENGTH)) {
                return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, 
                    !"Максимальная длина содержимого должна быть в диапазоне: от "#MIN_PLAYER_QM_LENGTH" до "#MAX_PLAYER_QM_LENGTH".");
            }

            return QM_PlayerCreate(playerid, inputtext);
        }
        case DIALOG_QM_DESCRIPTION: {
            if (!response) {
                return QM_ShowPlayerMainMenuDialog(playerid);
            }

            DeletePVar(playerid, pVar__QuickMessageIndex);
            return 1;
        }
        case DIALOG_QM_EDIT: {
            if (!response) {
                return QM_ShowPlayerMainMenuDialog(playerid);
            }

            new len = strlen(inputtext);

            if (!(MIN_PLAYER_QM_LENGTH <= len <= MAX_PLAYER_QM_LENGTH)) {
                return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, 
                    !"Максимальная длина содержимого должна быть в диапазоне: от "#MIN_PLAYER_QM_LENGTH" до "#MAX_PLAYER_QM_LENGTH".");
            }

            return QM_PlayerUpdate(playerid, inputtext);
        }
        case DIALOG_QM_DELETE: {
            if (!response) {
                return QM_ShowPlayerMainMenuDialog(playerid);
            }

            return QM_PlayerDelete(playerid);
        }
        case DIALOG_QM_START: {
            if (!response) {
                return QM_ShowPlayerMainMenuDialog(playerid);
            }
        }
    }

    #if defined QM_OnDialogResponse
        QM_OnDialogResponse(playerid, dialogid, response, listitem, inputtext);
    #endif
    return 0;
}
#if defined _ALS_OnDialogResponse
    #undef OnDialogResponse
#else
    #define _ALS_OnDialogResponse
#endif

#define OnDialogResponse QM_OnDialogResponse
#if defined QM_OnDialogResponse
    forward QM_OnDialogResponse(playerid, dialogid, response, listitem, inputtext[]);
#endif

/*  
    functions
*/

stock QM_SetMySQLConnectionHandle(const MySQL: handle)
{
    g_qm_sql_handle = handle;
}

stock MySQL: QM_GetMySQLConnectionHandle()
{
    return g_qm_sql_handle;
}

stock QM_Init()
{
    new Cache: cache_id = mysql_query(QM_GetMySQLConnectionHandle(), !"SHOW TABLES LIKE '"QM_TABLE"'");

    if (!cache_num_rows()) {
        mysql_query(QM_GetMySQLConnectionHandle(), 
            "\
                CREATE TABLE IF NOT EXISTS `"QM_TABLE"` ( \
                    `id` int(11) NOT NULL AUTO_INCREMENT, \
                    `slot` tinyint(3) NOT NULL, \
                    `user_id` int(11) NOT NULL, \
                    `title` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL, \
                    `message` varchar(128) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL, \
                    `unix_create` int(11) NOT NULL, \
                    `unix_last_edit` int(11) NOT NULL, \
                        PRIMARY KEY (`id`) \
                ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4; \
            ", false
        );
        mysql_query(QM_GetMySQLConnectionHandle(),
            !"\
                CREATE INDEX \
                    user_id \
                ON \
                    `"QM_TABLE"`(user_id) \
            ", false
        );
        mysql_query(QM_GetMySQLConnectionHandle(),
            !"\
                ALTER TABLE \
                    `"QM_TABLE"`\
                ADD CONSTRAINT \
                    `quick_message_users_fk_1` \
                FOREIGN KEY \
                    (`user_id`) \
                REFERENCES \
                    `"QM_TABLE_ACCOUNTS"` (`id`) \
                ON DELETE CASCADE ON UPDATE CASCADE \
            ", false
        );
    }
    cache_delete(cache_id);

    Iter_Init(iPlayerQuickMessage);

    return 1;
}

stock QM_PlayerMainClear(const playerid)
{
    g_player_qm_main_data[playerid] = NULL_g_player_qm_main_data;
}

stock QM_PlayerClear(const playerid, const index = -1)
{
    if (index == -1) {
        foreach (new i : iPlayerQuickMessage[playerid]) {
            g_player_qm_info[playerid][i] = NULL_g_player_qm_info;
        }

        Iter_Clear(iPlayerQuickMessage[playerid]);

        return 1;
    }

    return g_player_qm_info[playerid][index] = NULL_g_player_qm_info;
}

stock QM_SetPlayerUserID(const playerid, const value)
{
    g_player_qm_main_data[playerid][E_PLAYER_QM_MAIN_ID] = value;
}

stock QM_GetPlayerUserID(const playerid)
{
    return g_player_qm_main_data[playerid][E_PLAYER_QM_MAIN_ID];
}

stock QM_LoadPlayer(const playerid)
{
    new query_string[94 + (- 2 + 11) + (- 2 + 3) + 1];

    format(query_string, sizeof query_string,
        "\
            SELECT * FROM `"QM_TABLE"` qm \
            WHERE qm.user_id=%d ORDER by qm.slot ASC LIMIT %d \
        ",
            QM_GetPlayerUserID(playerid),
            MAX_PLAYER_QM_COUNT
    );
    mysql_tquery(QM_GetMySQLConnectionHandle(), query_string, !"@__OnLoadPlayerQuickMessage", !"d", playerid);

    return mysql_errno() == 0;
}

stock QM_ShowPlayerMainMenuDialog(const playerid)
{
    if (!Iter_Count(iPlayerQuickMessage[playerid])) {
        return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, !"Нет доступных быстрых сообщений.");
    }

    new dialog_string[(54 + (- 2 + 2) + (- 2 + MAX_PLAYER_QM_LENGTH)) + 1];

    foreach (new i : iPlayerQuickMessage[playerid]) {
        format(dialog_string, sizeof dialog_string, "%s{FFBF00}%d. {FFFFFF}%s\n",
            dialog_string,
            i + 1,
            g_player_qm_info[playerid][i][E_PLAYER_QM_TITLE]
        );
    }

    strcat(dialog_string, "> Добавить быстрое сообщение");

    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_MAIN_MENU,
        DIALOG_STYLE_LIST,
        !"{20997C}Быстрые сообщения",
        dialog_string,
        !"Выбрать", !"Закрыть"
    );
}

stock QM_ShowPlayerEditMenuDialog(const playerid, const index)
{
    SetPVarInt(playerid, pVar__QuickMessageIndex, index);

    new header_string[38 + (- 2 + 2) + 1];

    format(header_string, sizeof header_string, "{20997C}Быстрое сообщение: {FFFFFF}#%d", 
        index + 1
    );

    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_EDIT_MENU,
        DIALOG_STYLE_TABLIST_HEADERS,
        header_string,
        "\
            #\tДействие\n\
            {FFBF00}1.{FFFFFF}\t Описание\n\
            {FFBF00}2.{FFFFFF}\t Редактировать\n\
            {FFBF00}3.{FFFFFF}\t Удалить\n\
            {FFBF00}4.{FFFFFF}\t Запустить\
        ",
        !"Выбрать", !"Назад"
    );
}

stock QM_ShowPlayerOperateEditMenu(const playerid, const type) 
{
    switch (type) {
        case QM_EDIT_MENU_TYPE_DESCRIPTION: {
            return QM_ShowPlayerDescriptionDialog(playerid);
        }
        case QM_EDIT_MENU_TYPE_EDIT: {
            return QM_ShowPlayerEditDialog(playerid);
        }
        case QM_EDIT_MENU_TYPE_DELETE: {
            return QM_ShowPlayerDeleteDialog(playerid);
        }
        case QM_EDIT_MENU_TYPE_START: {
            return QM_ShowPlayerStartDialog(playerid);
        }
    }

    return 1;
}

stock QM_ShowPlayerTitleDialog(const playerid)
{
    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_TITLE,
        DIALOG_STYLE_INPUT,
        !"{20997C}Добавить: {FFFFFF}заголовок",
        !"{FFFFFF}Введите в поле ниже заголовок быстрого сообщения.\n\
        {FF0000}Примечание: максимальная длина заголовка должна быть в диапазоне: \
        от "#MIN_PLAYER_QM_TITLE_LENGTH" до "#MAX_PLAYER_QM_TITLE_LENGTH".",
        !"Далее", !"Назад"
    );
}

stock QM_ShowPlayerAddDialog(const playerid)
{
    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_CREATE,
        DIALOG_STYLE_INPUT,
        !"{20997C}Добавить: {FFFFFF}содержимое",
        !"{FFFFFF}Введите в поле ниже содержимое быстрого сообщения.\n\
        {FF0000}Примечание: максимальная длина содержимого должна быть в диапазоне: от \
        "#MIN_PLAYER_QM_LENGTH" до "#MAX_PLAYER_QM_LENGTH".",
        !"Добавить", !"Отмена"
    );
}

stock QM_ShowPlayerDescriptionDialog(const playerid) 
{
    new 
        dialog_string[205 + (- 2 + 2) + (- 2 + MAX_PLAYER_QM_LENGTH) + (- 2 + 20) + (- 2 + 20) + 1],
        index = GetPVarInt(playerid, pVar__QuickMessageIndex),
        unix_create[3],
        unix_last_edit[3];

    gmtime(g_player_qm_info[playerid][index][E_PLAYER_QM_UNIX_CREATE], 
        unix_create[0], unix_create[1], unix_create[2]
    );

    gmtime(g_player_qm_info[playerid][index][E_PLAYER_QM_UNIX_LAST_EDIT], 
            unix_last_edit[0], unix_last_edit[1], unix_last_edit[2]
    );

    format(dialog_string, sizeof dialog_string, 
        "\
            {FFFFFF}Слот: {FFBF00}%d{FFFFFF}\n\
            Содержимое: {FFBF00}%s{FFFFFF}\n\
            Дата создания: {FFBF00}%02d.%02d.%d{FFFFFF}\n\
            Дата последнего редактирования: {FFBF00}%02d.%02d.%d\
        ",
            index + 1,
            g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE],
            unix_create[2], unix_create[1], unix_create[0],
            unix_last_edit[2], unix_last_edit[1], unix_last_edit[0]
    );
    
    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_DESCRIPTION,
        DIALOG_STYLE_MSGBOX,
        !"{20997C}Описание быстрого сообщения",
        dialog_string,
        !"Закрыть", !"Назад"
    );
}

stock QM_ShowPlayerEditDialog(const playerid)
{
    new 
        dialog_string[295 + (- 2 + MAX_PLAYER_QM_LENGTH) + 1],
        index = GetPVarInt(playerid, pVar__QuickMessageIndex);

    format(dialog_string, sizeof dialog_string, 
        "\
            {FFFFFF}Введите в поле ниже содержимое быстрого сообщения.\n\
            {FF0000}Примечание: максимальная длина содержимого должна быть в диапазоне: от \
            "#MIN_PLAYER_QM_LENGTH" до "#MAX_PLAYER_QM_LENGTH".\n\n\
            {FFFFFF}Текущее содержимое быстрого сообщения:\n {FFBF00}%s\
        ",
            g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE]
    );

    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_EDIT,
        DIALOG_STYLE_INPUT,
        !"{20997C}Редактирование быстрого сообщения",
        dialog_string,
        !"Далее", !"Назад"
    );
}

stock QM_ShowPlayerDeleteDialog(const playerid)
{
    new 
        dialog_string[135 + (- 2 + 2) + (- 2 + MAX_PLAYER_QM_LENGTH) + 1],
        index = GetPVarInt(playerid, pVar__QuickMessageIndex);

    format(dialog_string, sizeof dialog_string, 
        "\
            {FFFFFF}Вы действительно хотите удалить быстрое сообщение (ID: #%d)?\n\
            Текущее содержимое быстрого сообщения:\n {FFBF00}%s\
        ",
            index + 1,
            g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE]
    );

    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_DELETE,
        DIALOG_STYLE_MSGBOX,
        !"{20997C}Удалить быстрое сообщение",
        dialog_string,
        !"Удалить", !"Назад"
    );
}

stock QM_ShowPlayerStartDialog(const playerid)
{
    new 
        dialog_string[139 + (- 2 + 2) + (- 2 + MAX_PLAYER_QM_LENGTH) + 1],
        index = GetPVarInt(playerid, pVar__QuickMessageIndex);

    format(dialog_string, sizeof dialog_string, 
        "\
            {FFFFFF}Вы действительно хотите запустить быстрое сообщение (ID: #%d)?\n\
            Текущее содержимое быстрого сообщения:\n {FFBF00}%s\
        ",
            index + 1,
            g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE]
    );

    return ShowPlayerDialog(
        playerid,
        DIALOG_QM_START,
        DIALOG_STYLE_MSGBOX,
        !"{20997C}Запустить быстрое сообщение",
        dialog_string,
        !"Запустить", !"Назад"
    );
}

stock QM_PlayerCreate(const playerid, const message[])
{
    new
        query_string[173 + (- 2 + 11) + (- 2 + 2) + (- 2 + MAX_PLAYER_QM_TITLE_LENGTH) + (- 2 + MAX_PLAYER_QM_LENGTH) + (- 2 + 11) + 1],
        unix_create = gettime(),
        index = GetPVarInt(playerid, pVar__QuickMessageIndex);

    DeletePVar(playerid, pVar__QuickMessageIndex);

    g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE] = NULL_g_player_qm_info[E_PLAYER_QM_MESSAGE];

    strmid(g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE], message, 0, strlen(message));

    Iter_Add(iPlayerQuickMessage[playerid], index);

    mysql_format(QM_GetMySQLConnectionHandle(), query_string, sizeof query_string,
        "\
            INSERT INTO `"QM_TABLE"` \
                (user_id, slot, title, message, unix_create, unix_last_edit) \
            VALUES \
                (%d, %d, '%e', '%e', %d, 0) \
        ",
            QM_GetPlayerUserID(playerid),
            index + 1,
            g_player_qm_info[playerid][index][E_PLAYER_QM_TITLE],
            message,
            unix_create
    );
    mysql_tquery(QM_GetMySQLConnectionHandle(), query_string, "", "");

    format(query_string, sizeof query_string, "Вы успешно добавили новое быстрое сообщение (ID: #%d).",
        index + 1
    );
    SendClientMessage(playerid, HEX_COLOR_QM_SUCCESS, query_string);

    return mysql_errno() == 0;
}

stock QM_PlayerUpdate(const playerid, const message[])
{
    new
        query_string[200 + (- 2 + MAX_PLAYER_QM_LENGTH) + (- 2 + 11) + (- 2 + 11) + (- 2 + 2) + 1],
        unix_last_edit = gettime(),
        index = GetPVarInt(playerid, pVar__QuickMessageIndex);

    DeletePVar(playerid, pVar__QuickMessageIndex);

    g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE] = NULL_g_player_qm_info[E_PLAYER_QM_MESSAGE];

    strmid(g_player_qm_info[playerid][index][E_PLAYER_QM_MESSAGE], message, 0, strlen(message));

    mysql_format(QM_GetMySQLConnectionHandle(), query_string, sizeof query_string,
        "\
            UPDATE \
                `"QM_TABLE"` qm \
            SET \
                qm.message='%e', \
                qm.unix_last_edit=%d \
            WHERE \
                qm.user_id=%d AND qm.slot=%d \
        ",
            message,
            unix_last_edit,
            QM_GetPlayerUserID(playerid),
            index + 1
    );
    mysql_tquery(QM_GetMySQLConnectionHandle(), query_string, "", "");

    format(query_string, sizeof query_string, "Вы успешно обновили содержимое быстрого сообщения (ID: #%d).",
        index + 1
    );
    SendClientMessage(playerid, HEX_COLOR_QM_SUCCESS, query_string);

    return mysql_errno() == 0;
}

stock QM_PlayerDelete(const playerid)
{
    new
        query_string[114 + (- 2 + 11) + (- 2 + 2) + 1],
        index = GetPVarInt(playerid, pVar__QuickMessageIndex);

    DeletePVar(playerid, pVar__QuickMessageIndex);

    QM_PlayerClear(playerid, index);
    
    Iter_Remove(iPlayerQuickMessage[playerid], index);

    format(query_string, sizeof query_string,
        "\
            DELETE FROM \
                `"QM_TABLE"` qm \
            WHERE \
                qm.user_id=%d AND qm.slot=%d \
        ",
            QM_GetPlayerUserID(playerid),
            index + 1
    );
    mysql_tquery(QM_GetMySQLConnectionHandle(), query_string, "", "");

    format(query_string, sizeof query_string, "Вы успешно удалили быстрое сообщение (ID: #%d).",
        index + 1
    );
    SendClientMessage(playerid, HEX_COLOR_QM_SUCCESS, query_string);

    return mysql_errno() == 0;
}

stock QM_PlayerStart(const playerid)
{
    new 
        buffer[2],
        index = GetPVarInt(playerid, pVar__QuickMessageIndex);

    DeletePVar(playerid, pVar__QuickMessageIndex);

    valstr(buffer, index);

    callcmd::qm_start(playerid, buffer);

    return 1;
}

stock QM_GetFormatedTag(
    const playerid, 
    const input_string[], 
    output_string[], 
    const size_input = sizeof input_string,
    const size_output = sizeof output_string
)
{
    strcat((output_string[0] = EOS, output_string), input_string, size_output);

    for (new i = 0, pos; i < sizeof g_qm_tag_info; ++i) {
        while ((pos = strfind(output_string, g_qm_tag_info[i][E_QM_TAG_NAME])) != -1) {
            strdel(output_string, pos, pos + g_qm_tag_info[i][E_QM_TAG_NAME]);

            switch (i) {
                case QM_TAG_NAME: {
                    strins(output_string, g_player_qm_main_data[playerid][E_PLAYER_QM_MAIN_NAME], pos, size_output);
                }
            }

            break;
        }
    }

    return 1;
}
        
stock QM_GetFormatedCommand(const command[])
{
    for (new i = 0; i < sizeof g_qm_command_info; ++i) {
        if (!strcmp(command, g_qm_command_info[i][E_QM_COMMAND_NAME])) {
            return i;
        }
    }

    return -1;
}

/*
    timestamp manipulation functions by ziggi
*/

#define EPOCH_YR        1970           
#define SECS_DAY        (24 * 60 * 60)
#define LEAPYEAR(%0)    (!((%0) % 4) && (((%0) % 100) || !((%0) % 400)))
#define YEARSIZE(%0)    (LEAPYEAR(%0) ? 366 : 365)

static const _ytab[2][12] = {
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
};

stock gmtime(timestamp, &year = EPOCH_YR, &month = 1, &day = 1, &hour = 0, &minute = 0, &second = 0)
{
	static dayclock;

	year = EPOCH_YR;
	dayclock = timestamp % SECS_DAY;
	day = timestamp / SECS_DAY;

	second = dayclock % 60;
	minute = (dayclock % 3600) / 60;
	hour = dayclock / 3600;

	while (day >= YEARSIZE(year)) {
		day -= YEARSIZE(year);
		year++;
	}

	month = 0;

	while (day >= _ytab[LEAPYEAR(year)][month]) {
		day -= _ytab[LEAPYEAR(year)][month];
		month++;
	}

	day++;
	month++;
}

/*  
    MySQL callbacks
*/

@__OnLoadPlayerQuickMessage(const playerid);
@__OnLoadPlayerQuickMessage(const playerid)
{
    new row_count = cache_num_rows();

    if (!row_count) {
        return 0;
    }
    
    for (new i = 0, slot; i < row_count; ++i) {
        cache_get_value_name_int(i, !"slot", slot);
        
        cache_get_value_name_int(i, !"unix_create", g_player_qm_info[playerid][slot - 1][E_PLAYER_QM_UNIX_CREATE]);
        cache_get_value_name_int(i, !"unix_last_edit", g_player_qm_info[playerid][slot - 1][E_PLAYER_QM_UNIX_LAST_EDIT]);

        cache_get_value_name(i, !"message", g_player_qm_info[playerid][slot - 1][E_PLAYER_QM_MESSAGE], MAX_PLAYER_QM_LENGTH);
        cache_get_value_name(i, !"title", g_player_qm_info[playerid][slot - 1][E_PLAYER_QM_TITLE], MAX_PLAYER_QM_TITLE_LENGTH);

        Iter_Add(iPlayerQuickMessage[playerid], slot - 1);
    }

    return 1;
}

/*  
    commands
*/

cmd:qm_menu(playerid)
{
    return QM_ShowPlayerMainMenuDialog(playerid);
}

cmd:qm_start(playerid, const params[])
{
    if (!Iter_Count(iPlayerQuickMessage[playerid])) {
        return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, !"Нет доступных быстрых сообщений.");
    }
    
    new slot;

    if (sscanf(params, !"d", slot)) {
        return SendClientMessage(playerid, HEX_COLOR_QM_SUCCESS, !"Используйте: /qm_start [слот]");
    } 

    if (!(1 <= slot <= MAX_PLAYER_QM_COUNT)) {
        return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, !"Укажите слот в диапазоне от: 1 до "#MAX_PLAYER_QM_COUNT".");
    }

    if (!Iter_Contains(iPlayerQuickMessage[playerid], slot - 1)) {
        return SendClientMessage(playerid, HEX_COLOR_QM_ERROR, !"У Вас отсутствует данное быстрое сообщение.");
    }

    new string[MAX_CHATBUBBLE_LENGTH + 1];
    
    QM_GetFormatedTag(playerid, g_player_qm_info[playerid][slot][E_PLAYER_QM_MESSAGE], string);
    QM_GetFormatedCommand(string);

#if defined PAWNCMD_INC_
    PC_EmulateCommand(playerid, string);
#elseif defined DC_CMD
    DC_CMD(playerid, string);
#else
    OnPlayerCommandText(playerid, string);
#endif

    return 1;
}