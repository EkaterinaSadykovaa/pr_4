#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    int sockfd,portno;
    struct sockaddr_in server_addr;
    char ip_address[16];
    char buffer[BUFFER_SIZE];

    strncpy(ip_address, argv[1], sizeof(ip_address) - 1);
    ip_address[sizeof(ip_address) - 1] = '\0';
    portno = atoi( argv[ 2 ] );
    // Создание сокета
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Ошибка создания сокета");
        exit(EXIT_FAILURE);
    }
    
    ip_address[16] = argv[ 1 ] ;
    if( ip_address == NULL )
    {
        fprintf( stderr, "ERROR, no such host\n" );
        exit( 0 );
    }

    // Настройка адреса сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portno);
    server_addr.sin_addr.s_addr = inet_addr(ip_address); // Используйте IP-адрес сервера

    // Подключение к серверу
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка подключения к серверу");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Подключено к серверу. Угадайте число от 1 до 100:\n");

    // Игровой цикл
    while (1) {
        // Запрос у пользователя ввода числа
        printf("Введите вашу догадку: ");
        fgets(buffer, BUFFER_SIZE, stdin); // Чтение ввода пользователя

        // Отправка догадки на сервер
        send(sockfd, buffer, strlen(buffer), 0);

        // Чтение ответа от сервера
        int bytes_read = read(sockfd, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) {
            perror("Ошибка чтения или сервер отключился");
            break;
        }
        buffer[bytes_read] = '\0'; // Завершение строки

        // Вывод ответа от сервера
        printf("Ответ от сервера: %s", buffer);

        // Проверка условия выхода из цикла
        if (strstr(buffer, "Поздравляем!") != NULL) {
            break; // Выход из цикла, если угадали
        }
    }

    // Закрытие соединения
    close(sockfd);
    return 0;
}

