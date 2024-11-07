#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ETHSIZE 800
#define PORT 7780
#define CUPOTOTAL 30  // Cupo total fijo para el grupo
#define MAX_SOLICITUDES 100  // Máximo número de solicitudes

struct Solicitud {
    char nombre[50];
    char claveUEA[10];
    char grupo[10];
};

struct Solicitud solicitudes[MAX_SOLICITUDES];  // Arreglo para almacenar las solicitudes
int num_solicitudes = 0;  // Contador de solicitudes recibidas

struct sockaddr_in serv, cli;

int main() {
    int fd, client_fd, cli_len, size_recv;
    char request[ETHSIZE];
    char response[100];  // Respuesta para el cliente

    printf("\nSe creará el socket...\n");
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("\nError, no se pudo crear el socket.\n");
        exit(1);
    }
    printf("\nSocket creado.\n");

    printf("\nSe asignarán los atributos al socket...\n");
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(PORT);

    if (bind(fd, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        printf("\nNo se asignaron los atributos.\n");
        exit(1);
    }
    printf("\nSí se asignaron los atributos.\n");

    // Escuchar conexiones entrantes
    listen(fd, 5);
    printf("\nEsperando conexiones en el puerto %d...\n", PORT);

    for (;;) {  // Ciclo infinito para aceptar conexiones
        cli_len = sizeof(cli);
        client_fd = accept(fd, (struct sockaddr *)&cli, (socklen_t *)&cli_len);
        if (client_fd < 0) {
            printf("\nError en la conexión del cliente.\n");
            continue;
        }
        printf("\nCliente conectado.\n");

        // Recibir mensaje del cliente
        size_recv = recv(client_fd, request, ETHSIZE, 0);
        if (size_recv < 0) {
            printf("\nError en recv.\n");
            close(client_fd);
            continue;
        }

        // Decodificar la solicitud recibida
        struct Solicitud nueva_solicitud;
        sscanf(request, "%s %s %s", nueva_solicitud.nombre, nueva_solicitud.claveUEA, nueva_solicitud.grupo);

        // Almacenar la solicitud en el arreglo
        if (num_solicitudes < MAX_SOLICITUDES) {
            solicitudes[num_solicitudes++] = nueva_solicitud;

            // Mostrar la lista actual de solicitudes
            printf("\nLista de solicitudes de inscripción:\n");
            for (int i = 0; i < num_solicitudes; i++) {
                printf("Solicitud %d: Nombre: %s, Clave UEA: %s, Grupo: %s\n", 
                       i + 1, solicitudes[i].nombre, solicitudes[i].claveUEA, solicitudes[i].grupo);
            }

            // Preparar y enviar la respuesta al cliente
            sprintf(response, "%d %d", CUPOTOTAL, num_solicitudes);
            send(client_fd, response, strlen(response), 0);
        } else {
            printf("\nSe alcanzó el límite de solicitudes.\n");
        }

        // Cerrar la conexión con el cliente
        close(client_fd);
        printf("\nCliente desconectado.\n");
    }

    close(fd);
    return 0;
}
