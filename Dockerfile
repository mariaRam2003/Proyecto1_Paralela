# Utiliza una imagen base de Ubuntu
FROM ubuntu:latest

# Instala las dependencias necesarias
RUN apt-get update && apt-get install -y gcc g++ make libsdl2-dev

# Establece el directorio de trabajo
WORKDIR /usr/src

# Copia tu código fuente al contenedor
COPY . .

# Compila tu programa (opcional, si deseas compilarlo en la imagen)
RUN gcc -o screensaver screensaver.c -lSDL2 -lm -fopenmp
