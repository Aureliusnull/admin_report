#if defined _admin_report_included
    #endinput
#endif
#define _admin_report_included

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

#if !defined ADMIN_REPORT_HEX_COLOR
	#define ADMIN_REPORT_HEX_COLOR 0x3399FFFF 	
#endif

#if !defined GetPlayerNameEx
    #define GetPlayerNameEx(%0) PlayerInfo[%0][pName] 
#endif

#if !defined GetPlayerAdminEx
    #define GetPlayerAdminEx(%0) PlayerInfo[%0][pAdmin]
#endif

enum E_ADMIN_REPORT_STRUCT {
    AR_ID,
    AR_MESSAGE[MAX_ADMIN_REPORT_MESSAGE_LENGTH + 1],
    AR_UNIX_TIME
};

new 
    g_admin_report[MAX_ADMIN_REPORT_COUNT][E_ADMIN_REPORT_STRUCT],
    NULL_g_admin_report[E_ADMIN_REPORT_STRUCT] = {
    INVALID_PLAYER_ID,
    EOS,
    -1
};

new 
    Iterator: g_admin_report_iterator<MAX_ADMIN_REPORT_COUNT>,
    pVar__AdminReport_IDX[] = !"pVar__AdminReport_IDX",
    g_player_admin_report_delay[MAX_PLAYERS] = { -1, ...};
    g_player_admin_report_listitem[MAX_PLAYERS][MAX_ADMIN_REPORT_LISTITEM] = { { 0, ...}, ...};