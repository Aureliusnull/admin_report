# admin_report 

После подключения всех необходимых библиотек:
```cpp
#include <a_samp>
#include <admin_report.inc>
```
Если игрок является администратором, после загрузки данных аккаунта необходимо определить его привилегии.

Функция: 
```cpp
stock SetPlayerAdminReportLevel(const playerid, const value)
```
Аргументы:
```cpp
playerid - идентификатор игрока.
value - значение уровня администратора.
```

Пример:
```cpp
public PlayerLogin(playerid)
{
    new rows, tmp_admin_level;
    cache_get_row_count(rows);

    if (rows)
    {
        cache_get_value_name_int(0, "admin", tmp_admin_level);

        if (tmp_admin_level) 
        {
            SetPlayerAdminReportLevel(playerid, tmp_admin_level);
        }
    }
    
    return 1;
}
```
