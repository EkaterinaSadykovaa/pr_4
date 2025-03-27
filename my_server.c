#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd, new_sock, portno;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int number_to_guess, guess;

    // Инициализация генератора случайных чисел
    srand(time(NULL));
    number_to_guess = rand() % 100 + 1; // Генерация числа от 1 до 100
    fprintf( stdout, "Загаданное %d число\n", number_to_guess );
    
    if( argc < 2 )
    {
        fprintf( stderr, "ERROR, no port provided\n" );
        exit(1);
    }
    portno = atoi( argv[ 2 ] );

    // Создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    portno = atoi( argv[ 1 ] );
    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    // Привязка сокета к адресу
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Установка режима прослушивания
    if (listen(sockfd, 5) < 0) {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Сервер слушает на порту %d...\n", portno);

    // Прием входящего соединения
    new_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
    if (new_sock < 0) {
        perror("Accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Клиент подключен: %s:%d\n",
           inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    // Игровой цикл
    while (1) {
        // Чтение догадки клиента
        int bytes_read = read(new_sock, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            perror("Read failed or client disconnected");
            break;
        }
        buffer[bytes_read] = '\0'; // Завершение строки

        // Преобразование догадки в число
        guess = atoi(buffer);
        printf("Клиент ввёл: %d\n", guess);

        // Проверка догадки
        if (guess < number_to_guess) {
            strcpy(buffer, "Слишком низко!\n");
        } else if (guess > number_to_guess) {
            strcpy(buffer, "Слишком высоко!\n");
        } else {
            strcpy(buffer, "Поздравляем! Вы угадали число!\n");
            send(new_sock, buffer, strlen(buffer), 0);
            break; // Выход из цикла, если угадали
        }

        // Отправка подсказки клиенту
        send(new_sock, buffer, strlen(buffer), 0);
    }

    // Закрытие соединения
    close(new_sock);
    close(sockfd);
    return 0;
}

