#include "client_manager.h"

#include <stdexcept>

ClientManager::ClientManager(pqxx::connection& connection) : conn_(connection) {}

void ClientManager::createTables() {
    pqxx::work tx{conn_};
    tx.exec(R"(
        CREATE TABLE IF NOT EXISTS client (
            id         SERIAL       PRIMARY KEY,
            first_name VARCHAR(60)  NOT NULL,
            last_name  VARCHAR(60)  NOT NULL,
            email      VARCHAR(120) NOT NULL UNIQUE
        )
    )");
    // Телефоны вынесены в отдельную таблицу: у клиента их может быть сколько угодно
    tx.exec(R"(
        CREATE TABLE IF NOT EXISTS phone (
            id        SERIAL      PRIMARY KEY,
            client_id INTEGER     NOT NULL REFERENCES client(id) ON DELETE CASCADE,
            number    VARCHAR(30) NOT NULL,
            UNIQUE (client_id, number)
        )
    )");
    tx.commit();
}

void ClientManager::ensureClientExists(pqxx::transaction_base& tx, int client_id) {
    pqxx::result r = tx.exec_params("SELECT 1 FROM client WHERE id = $1", client_id);
    if (r.empty()) {
        throw std::runtime_error("Клиент с id=" + std::to_string(client_id) + " не найден");
    }
}

int ClientManager::addClient(const std::string& first_name,
                             const std::string& last_name,
                             const std::string& email,
                             const std::vector<std::string>& phones) {
    pqxx::work tx{conn_};
    int id = tx.exec_params1(
                   "INSERT INTO client (first_name, last_name, email) "
                   "VALUES ($1, $2, $3) RETURNING id",
                   first_name, last_name, email)[0]
                 .as<int>();
    for (const auto& p : phones) {
        tx.exec_params("INSERT INTO phone (client_id, number) VALUES ($1, $2)", id, p);
    }
    tx.commit();
    return id;
}

void ClientManager::addPhone(int client_id, const std::string& phone) {
    pqxx::work tx{conn_};
    ensureClientExists(tx, client_id);
    tx.exec_params("INSERT INTO phone (client_id, number) VALUES ($1, $2)", client_id, phone);
    tx.commit();
}

void ClientManager::updateClient(int client_id,
                                 const std::optional<std::string>& first_name,
                                 const std::optional<std::string>& last_name,
                                 const std::optional<std::string>& email) {
    pqxx::work tx{conn_};
    ensureClientExists(tx, client_id);
    // COALESCE оставляет прежнее значение, если параметр не передан (NULL)
    tx.exec_params(
        "UPDATE client SET "
        "first_name = COALESCE($2, first_name), "
        "last_name  = COALESCE($3, last_name), "
        "email      = COALESCE($4, email) "
        "WHERE id = $1",
        client_id, first_name, last_name, email);
    tx.commit();
}

void ClientManager::deletePhone(int client_id, const std::string& phone) {
    pqxx::work tx{conn_};
    pqxx::result r = tx.exec_params(
        "DELETE FROM phone WHERE client_id = $1 AND number = $2", client_id, phone);
    if (r.affected_rows() == 0) {
        throw std::runtime_error("Телефон " + phone + " у клиента id=" +
                                 std::to_string(client_id) + " не найден");
    }
    tx.commit();
}

void ClientManager::deleteClient(int client_id) {
    pqxx::work tx{conn_};
    pqxx::result r = tx.exec_params("DELETE FROM client WHERE id = $1", client_id);
    if (r.affected_rows() == 0) {
        throw std::runtime_error("Клиент с id=" + std::to_string(client_id) + " не найден");
    }
    tx.commit();
}

std::vector<Client> ClientManager::findClients(const std::optional<std::string>& first_name,
                                               const std::optional<std::string>& last_name,
                                               const std::optional<std::string>& email,
                                               const std::optional<std::string>& phone) {
    pqxx::work tx{conn_};
    // Критерий, равный NULL, игнорируется. Телефон ищем через EXISTS,
    // чтобы в результате остались все телефоны найденного клиента.
    pqxx::result rows = tx.exec_params(
        "SELECT c.id, c.first_name, c.last_name, c.email, "
        "       COALESCE(string_agg(p.number, ',' ORDER BY p.id), '') AS phones "
        "FROM client c "
        "LEFT JOIN phone p ON p.client_id = c.id "
        "WHERE ($1::text IS NULL OR c.first_name = $1) "
        "  AND ($2::text IS NULL OR c.last_name  = $2) "
        "  AND ($3::text IS NULL OR c.email      = $3) "
        "  AND ($4::text IS NULL OR EXISTS (SELECT 1 FROM phone px "
        "                                   WHERE px.client_id = c.id AND px.number = $4)) "
        "GROUP BY c.id "
        "ORDER BY c.id",
        first_name, last_name, email, phone);

    std::vector<Client> result;
    for (const auto& row : rows) {
        Client c;
        c.id = row["id"].as<int>();
        c.first_name = row["first_name"].as<std::string>();
        c.last_name = row["last_name"].as<std::string>();
        c.email = row["email"].as<std::string>();
        std::string joined = row["phones"].as<std::string>();
        size_t start = 0;
        while (!joined.empty() && start <= joined.size()) {
            size_t pos = joined.find(',', start);
            if (pos == std::string::npos) pos = joined.size();
            c.phones.push_back(joined.substr(start, pos - start));
            start = pos + 1;
        }
        result.push_back(std::move(c));
    }
    tx.commit();
    return result;
}
