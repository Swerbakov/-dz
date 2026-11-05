# Управление клиентами: C++ + PostgreSQL (libpqxx)

## Структура БД
- `client` (id, first_name, last_name, email UNIQUE)
- `phone` (id, client_id → client.id ON DELETE CASCADE, number) — у клиента может быть 0, 1 или много телефонов.

## Сборка и запуск
```bash
# Ubuntu: sudo apt install g++ cmake libpqxx-dev
createdb -U postgres clients_db        # база должна существовать
cmake -B build && cmake --build build
./build/clients_db
```
Параметры подключения по умолчанию: `localhost:5432`, БД `clients_db`, пользователь `postgres`, пароль `postgres`.
Переопределяются переменными окружения `DB_HOST`, `DB_PORT`, `DB_NAME`, `DB_USER`, `DB_PASSWORD`.

## Методы класса `ClientManager`
`createTables`, `addClient`, `addPhone`, `updateClient`, `deletePhone`, `deleteClient`, `findClients`.
