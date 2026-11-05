#pragma once

#include <optional>
#include <string>
#include <vector>

#include <pqxx/pqxx>

// Данные клиента, возвращаемые при поиске
struct Client {
    int id;
    std::string first_name;
    std::string last_name;
    std::string email;
    std::vector<std::string> phones;  // у клиента может быть 0, 1 или несколько телефонов
};

// Класс для управления клиентами в PostgreSQL
class ClientManager {
public:
    explicit ClientManager(pqxx::connection& connection);

    // 1. Создаёт структуру БД (таблицы)
    void createTables();

    // 2. Добавляет нового клиента (телефоны необязательны). Возвращает id клиента
    int addClient(const std::string& first_name,
                  const std::string& last_name,
                  const std::string& email,
                  const std::vector<std::string>& phones = {});

    // 3. Добавляет телефон существующему клиенту
    void addPhone(int client_id, const std::string& phone);

    // 4. Изменяет данные клиента: переданные (не std::nullopt) поля обновляются
    void updateClient(int client_id,
                      const std::optional<std::string>& first_name = std::nullopt,
                      const std::optional<std::string>& last_name = std::nullopt,
                      const std::optional<std::string>& email = std::nullopt);

    // 5. Удаляет телефон у существующего клиента
    void deletePhone(int client_id, const std::string& phone);

    // 6. Удаляет существующего клиента (его телефоны удаляются каскадно)
    void deleteClient(int client_id);

    // 7. Ищет клиентов по имени, фамилии, email или телефону.
    //    Можно указать любую комбинацию критериев (они объединяются через И).
    std::vector<Client> findClients(const std::optional<std::string>& first_name = std::nullopt,
                                    const std::optional<std::string>& last_name = std::nullopt,
                                    const std::optional<std::string>& email = std::nullopt,
                                    const std::optional<std::string>& phone = std::nullopt);

private:
    void ensureClientExists(pqxx::transaction_base& tx, int client_id);

    pqxx::connection& conn_;
};
