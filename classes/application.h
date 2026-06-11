#ifndef APPLICATION_H
#define APPLICATION_H

/**
 * @brief Основной класс приложения
 * 
 * Координирует работу всех компонентов приложения и обрабатывает командную строку.
 */
class Application {
public:
    /**
     * @brief Основной метод запуска приложения
     * @param argc Количество аргументов командной строки
     * @param argv Массив аргументов командной строки
     * @return Код выхода приложения (0 - успех, не 0 - ошибка)
     */
    int run(int argc, char* argv[]);
};

#endif