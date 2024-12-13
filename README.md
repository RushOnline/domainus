# Domainus

Телеграм-бот для управление списком доменов, маршрутизируемых роутером через `VPN`.

Для запуска необходима установка следующих переменных окружения:

- `DOMAINUS_TOKEN`: токен телеграм-бота
- `DOMAINUS_DBPATH`: путь к файлу с доменами

# Формат сообщения

## Добавление URL/доменов

Запрос - список URL/доменов, элементы списка разделяются пробелом или переносом строки, например:
```
discord.app
discordapp.com
discordapp.net
discordapp.io
discordapp.org
discordcdn.com
```

Ответ - сообщение об успехе или ошибке, например:
```
[V] discord.app
[X] discordapp.com
[V] discordapp.net
[V] discordapp.io
[V] discordapp.org
[V] discordcdn.com
```

## Удаление доменов

Запрос начинается с команды "удали", за которой следует список доменов, элементы списка разделяются
пробелом или переносом строки, например:

```
удали rutracker.org
```

```
удали
discord.app
discordapp.com
discordapp.net
discordapp.io
discordapp.org
discordcdn.com
```

Ответ: сообщение об успехе или ошибке.

## Поиск среди добавленных доменов

Запрос начинается с команды "ищи", за которой следует подстрока, которую следует искать, например:

```
ищи discord
```

Ответ - список доменов, содержащих указанную подстроку, например:

```
discord.app
discordapp.com
discordapp.net
discordapp.io
discordapp.org
discordcdn.com
```
