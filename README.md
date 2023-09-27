# QuickMessage - быстрые сообщения 
## Установка
- Переместить файл quick_message.inc в папку с инклудами игрового мода (в большинстве случаев, это: /pawno/include).
- В используемом скрипте подключить инклуд quick_message.inc и указать все необходимые настройки (см. ниже).
```pawn
#include <a_samp>
#include <quick_message>
```
- В используемом скрипте необходимо объявить следующие функции:
```pawn
/* В public OnGameModeInit() после подключения к БД. */
QM_SetMySQLConnectionHandle(/* ID соединения с БД */)
```
```pawn
/* В public OnPlayerSpawn(playerid) */
QM_SetPlayerUserID(playerid, /* Переменная хранящая ID аккаунта в таблице. */);
QM_LoadPlayer(playerid);
```
- В директиве QM_TABLE_ACCOUNTS:

Укажите название своей основной таблицы с аккаунтами игроков, если она отличается от стандартной.
