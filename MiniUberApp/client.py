import sys
import socket
import time

# Verificar argumentos necesarios
if len(sys.argv) < 4:
    print("Uso: python clientetcp.py <ip_servidor> <puerto_servidor> <tipo_solicitud>")
    sys.exit(1)

# Obtener IP del servidor, puerto y tipo de solicitud desde los argumentos
ip_servidor = sys.argv[1]
puerto_servidor = int(sys.argv[2])
tipo_solicitud = sys.argv[3]  # 'estado', 'viaje' o 'viaje_terminado'

# Argumento opcional para 'viaje_terminado' para especificar la placa del auto
placa_auto = None
if tipo_solicitud == "viaje_terminado" and len(sys.argv) == 5:
    placa_auto = sys.argv[4]

# Conectar con el servidor
direccion_servidor = (ip_servidor, puerto_servidor)
tamano_buffer = 1024

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as cliente_socket:
    cliente_socket.connect(direccion_servidor)
    print("Conectado al servidor.")

    # Manejar la solicitud según el tipo
    if tipo_solicitud == "estado":
        # Solicitar estado del servidor
        cliente_socket.sendall(tipo_solicitud.encode())
    
    elif tipo_solicitud == "viaje":
        # Solicitar un viaje
        cliente_socket.sendall(tipo_solicitud.encode())
    
    elif tipo_solicitud == "viaje_terminado":
        if placa_auto is None:
            print("Por favor especifica el número de placa para 'viaje_terminado'.")
            sys.exit(1)
        # Formatear solicitud con número de placa
        mensaje = f"{tipo_solicitud} {placa_auto}"
        cliente_socket.sendall(mensaje.encode())
    
    else:
        print("Tipo de solicitud inválido. Usa 'estado', 'viaje' o 'viaje_terminado'.")
        sys.exit(1)

    # Recibir respuesta del servidor
    respuesta = cliente_socket.recv(tamano_buffer).decode()
    print("Respuesta del servidor:", respuesta)

    # Simular duración del viaje si la solicitud fue para un 'viaje'
    if tipo_solicitud == "viaje" and "No hay conductores" not in respuesta:
        placa, costo = respuesta.split(', ')[0].split(': ')[1], respuesta.split(', ')[1].split(': ')[1]
        print(f"Placa del auto: {placa}, Costo del viaje: {costo}")
