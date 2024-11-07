import sys
import socket

# Verificar que se ingresen los argumentos necesarios
if len(sys.argv) != 3:
    print("Uso: python cliente.py <IP_servidor> <Puerto_servidor>")
    sys.exit(1)

# Obtener IP y puerto del servidor desde los argumentos
ip_servidor = sys.argv[1]
puerto_servidor = int(sys.argv[2])

# Solicitar los demás datos al usuario
nombre = input("Ingrese su nombre: ")
clave_UEA = input("Ingrese la clave de la UEA: ")
grupo = input("Ingrese el grupo: ")

# Crear el mensaje codificado
mensaje = f"{nombre} {clave_UEA} {grupo}"
server_address = (ip_servidor, puerto_servidor)
buffer_size = 1024  # Tamaño del buffer de recepción

# Crear socket UDP
cliente_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

try:
    # Enviar mensaje al servidor
    cliente_socket.sendto(mensaje.encode(), server_address)
    print(f"Mensaje enviado: {mensaje}")

    # Recibir respuesta del servidor
    data, server = cliente_socket.recvfrom(buffer_size)
    respuesta = data.decode()

    # Procesar y mostrar la respuesta
    cupo_total, num_solicitud = respuesta.split()
    print(f"Respuesta del servidor - Cupo Total: {cupo_total}, Número de Solicitud: {num_solicitud}")

finally:
    # Cerrar el socket
    cliente_socket.close()
