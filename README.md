# QuickMessage - быстрые сообщения 
## Установка
- Переместить файл quick_message.inc в папку с инклудами игрового мода (в большинстве случаев, это: /pawno/include).
- В используемом скрипте подключить инклуд quick_message.inc и указать все необходимые библиотеки (см. ниже):
```pawn
#include <a_samp>
#include <a_mysql>
#include <foreach>
#include <Pawn.CMD>
// #include <dc_cmd>
#include <quick_message>
```
- В используемом скрипте необходимо объявить следующие функции/директивы (см. ниже):
```pawn
/* Изменить наименование SQL таблицы с игровыми аккаунтами, если оно отличается от стандартного. */
#define QM_TABLE_ACCOUNTS "accounts"
```
```pawn
/* Добавить в public OnGameModeInit() после подключения к БД. */
QM_SetMySQLConnectionHandle(/* ID соединения с БД */)
```
```pawn
/* Добавить в public OnPlayerSpawn(playerid) */
QM_SetPlayerUserID(playerid, /* Переменная содержащая ID аккаунта в таблице. */);
QM_LoadPlayer(playerid);
```
## Использование:
- /qm_menu - открыть меню быстрых сообщений.
- /qm_start - отправить быстрое сообщение.
