#include "application.h"

/**
 * @brief Главная функция приложения
 * 
 * Создает экземпляр приложения и запускает его.
 * 
 * @param argc Количество аргументов командной строки
 * @param argv Массив аргументов командной строки
 * @return Код выхода программы (0 - успех, не 0 - ошибка)
 */
int main(int argc, char* argv[]) {
    Application application;
    return application.run(argc, argv);
}
