#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ETHSIZE   400
#define PORT 7780

struct sockaddr_in serv, cli;

int main() {
   int fd, client_fd, cli_len, size_recv;
   char request[ETHSIZE];
   char datos_para_el_cliente[] = "El clima para hoy se anuncia nublado";

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
      printf("\nEl request recibido fue: '%s'\n\n", request);

      // Enviar respuesta al cliente
      send(client_fd, datos_para_el_cliente, strlen(datos_para_el_cliente), 0);

      // Cerrar la conexión con el cliente después de enviar la respuesta
      close(client_fd);
      printf("\nCliente desconectado.\n");
   }

   close(fd);
   return 0;
}
