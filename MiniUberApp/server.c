#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define NUM_AUTOS 5
#define PUERTO 7780

int autos_disponibles[NUM_AUTOS] = {1, 1, 1, 1, 1};  // 1 disponible, 0 ocupado
int total_viajes = 0;
int ganancia_total = 0;

// Función para manejar solicitudes de los clientes
void manejar_solicitud(int client_fd) {
    char solicitud[100];
    char respuesta[100];
    int tamano_lectura;

    // Recibir solicitud
    tamano_lectura = recv(client_fd, solicitud, sizeof(solicitud), 0);
    solicitud[tamano_lectura] = '\0';

    if (strcmp(solicitud, "estado") == 0) {
        // Estado del servidor con etiquetas descriptivas
        snprintf(respuesta, sizeof(respuesta), "Viajes: %d, Ganancia Total: $%d", total_viajes, ganancia_total);

    } else if (strcmp(solicitud, "viaje") == 0) {
        int autos_libres[NUM_AUTOS];
        int num_libres = 0;

        // Crear lista de autos disponibles
        for (int i = 0; i < NUM_AUTOS; i++) {
            if (autos_disponibles[i] == 1) {
                autos_libres[num_libres] = i;
                num_libres++;
            }
        }

        if (num_libres > 0) {
            // Seleccionar un auto disponible aleatoriamente
            int indice_aleatorio = rand() % num_libres;
            int auto_encontrado = autos_libres[indice_aleatorio];
            autos_disponibles[auto_encontrado] = 0;  // Marcar como ocupado

            // Generar costo y actualizar estadísticas
            int costo_viaje = rand() % 100 + 50;  // Costo entre 50 y 150
            total_viajes++;
            ganancia_total += costo_viaje;

            snprintf(respuesta, sizeof(respuesta), "Placa: %d, Costo: $%d", auto_encontrado, costo_viaje);
        } else {
            snprintf(respuesta, sizeof(respuesta), "No hay conductores");
        }

    } else if (strncmp(solicitud, "viaje_terminado", 15) == 0) {
        // Terminación del viaje
        int placa;
        sscanf(solicitud + 16, "%d", &placa);  // Extraer número de placa
        autos_disponibles[placa] = 1;  // Marcar como disponible
        snprintf(respuesta, sizeof(respuesta), "Viaje terminado, auto %d disponible", placa);
    }

    // Enviar respuesta
    send(client_fd, respuesta, strlen(respuesta), 0);

    // Mostrar IP del cliente para propósitos de depuración
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    getpeername(client_fd, (struct sockaddr*)&client_addr, &client_len);
    printf("IP del cliente: %s\n", inet_ntoa(client_addr.sin_addr));

    // Cerrar conexión
    close(client_fd);
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    srand(time(0));  // Semilla para aleatoriedad

    // Crear socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Error en la creación del socket");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configurar dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PUERTO);

    // Asignar socket
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en la asignación");
        close(server_fd);
        exit(1);
    }

    // Escuchar conexiones
    listen(server_fd, 5);
    printf("Servidor escuchando en el puerto %d\n", PUERTO);

    while (1) {
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("Error en la aceptación de conexión");
            continue;
        }
        manejar_solicitud(client_fd);  // Manejar solicitud del cliente
    }

    close(server_fd);
    return 0;
}
