#include <cstdlib>
#include <iostream>

#include "client_manager.h"

static void printClients(const std::string& title, const std::vector<Client>& clients) {
    std::cout << "--- " << title << " ---\n";
    if (clients.empty()) std::cout << "(ничего не найдено)\n";
    for (const auto& c : clients) {
        std::cout << "#" << c.id << " " << c.first_name << " " << c.last_name
                  << " <" << c.email << "> телефоны: ";
        if (c.phones.empty()) std::cout << "нет";
        for (size_t i = 0; i < c.phones.size(); ++i)
            std::cout << (i ? ", " : "") << c.phones[i];
        std::cout << "\n";
    }
    std::cout << "\n";
}

static std::string env(const char* name, const char* def) {
    const char* v = std::getenv(name);
    return v ? v : def;
}

int main() {
    try {
        // Параметры подключения можно переопределить переменными окружения
        std::string conn_str = "host=" + env("DB_HOST", "localhost") +
                               " port=" + env("DB_PORT", "5432") +
                               " dbname=" + env("DB_NAME", "clients_db") +
                               " user=" + env("DB_USER", "postgres") +
                               " password=" + env("DB_PASSWORD", "postgres");
        pqxx::connection conn(conn_str);
        ClientManager manager(conn);

        // 1. Создание таблиц
        manager.createTables();
        std::cout << "Таблицы созданы\n\n";

        // 2. Добавление клиентов (с телефонами и без)
        int ivan = manager.addClient("Иван", "Петров", "ivan@example.com", {"+7 900 111-11-11"});
        int anna = manager.addClient("Анна", "Сидорова", "anna@example.com");
        int oleg = manager.addClient("Олег", "Петров", "oleg@example.com",
                                     {"+7 900 222-22-22", "+7 900 333-33-33"});
        printClients("После добавления", manager.findClients());

        // 3. Добавление телефона существующему клиенту
        manager.addPhone(anna, "+7 900 444-44-44");
        manager.addPhone(ivan, "+7 900 555-55-55");
        printClients("После добавления телефонов", manager.findClients());

        // 4. Изменение данных клиента (только указанные поля)
        manager.updateClient(anna, std::nullopt, "Иванова", "anna.ivanova@example.com");
        manager.updateClient(ivan, "Иоанн");
        printClients("После изменения данных", manager.findClients());

        // 5. Удаление телефона
        manager.deletePhone(oleg, "+7 900 222-22-22");
        printClients("После удаления телефона у Олега", manager.findClients());

        // 7. Поиск по имени, фамилии, email и телефону
        printClients("Поиск по фамилии 'Петров'",
                     manager.findClients(std::nullopt, "Петров"));
        printClients("Поиск по имени 'Анна'", manager.findClients("Анна"));
        printClients("Поиск по email 'oleg@example.com'",
                     manager.findClients(std::nullopt, std::nullopt, "oleg@example.com"));
        printClients("Поиск по телефону '+7 900 555-55-55'",
                     manager.findClients(std::nullopt, std::nullopt, std::nullopt,
                                         "+7 900 555-55-55"));

        // 6. Удаление клиента (телефоны удаляются каскадно)
        manager.deleteClient(oleg);
        printClients("После удаления Олега", manager.findClients());

        // Обработка ошибки: несуществующий клиент
        try {
            manager.addPhone(9999, "+7 900 000-00-00");
        } catch (const std::exception& e) {
            std::cout << "Ожидаемая ошибка: " << e.what() << "\n\n";
        }

        // Очистка демонстрационных данных, чтобы программу можно было запускать повторно
        manager.deleteClient(ivan);
        manager.deleteClient(anna);
        std::cout << "Демонстрация завершена\n";
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
