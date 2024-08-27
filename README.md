# Proyecto #1 - Computación Paralela: Screensaver con Curvas de Lissajous

## Requerimientos

### Para Windows:

1. **Instalar Xming:**
   - Descarga Xming desde [este enlace](https://sourceforge.net/projects/xming/).
   - Instala Xming en tu sistema siguiendo las instrucciones del instalador.
   - Ejecuta Xming para iniciar el servidor X.

2. **Configurar Docker para Usar Xming:**
   - Establece la variable de entorno `DISPLAY` en PowerShell:
     ```powershell
     $env:DISPLAY="host.docker.internal:0.0"
     ```

   - Crea la nueva imagen Docker:
     ```powershell
     docker build -t proyecto_paralela .
     ```

   - Ejecuta el contenedor Docker con el siguiente comando:
     ```powershell
     docker run -it --rm --name mi_contenedor -e DISPLAY=$env:DISPLAY proyecto_paralela /bin/bash
     ```

### Para macOS:

1. **Instalar XQuartz:**
   - Descarga XQuartz desde [aquí](https://www.xquartz.org/).
   - Instala XQuartz y reinicia tu sistema si es necesario.

2. **Configurar Docker para Usar XQuartz:**
   - Abre la aplicación XQuartz.
   - Configura XQuartz para permitir conexiones desde clientes:
     - Abre XQuartz.
     - Ve a `XQuartz` > `Preferences` > `Security`.
     - Marca la opción `Allow connections from network clients`.
   - En la terminal de macOS, configura la variable `DISPLAY`:
     ```bash
     export DISPLAY=:0
     ```

   - Ejecuta el contenedor Docker con el siguiente comando:
     ```bash
     docker run -it --rm --name mi_contenedor -e DISPLAY=$DISPLAY proyecto_paralela /bin/bash
     ```

### Para Docker:

1. **Construir la Imagen Docker:**
   - Usa el siguiente `Dockerfile` para construir la imagen:
     ```dockerfile
     # Utiliza una imagen base de Ubuntu
     FROM ubuntu:latest

     # Instala las dependencias necesarias
     RUN apt-get update && apt-get install -y gcc g++ make libsdl2-dev libomp-dev

     # Establece el directorio de trabajo
     WORKDIR /usr/src

     # Copia tu código fuente al contenedor
     COPY . .

     # Compila tu programa
     RUN gcc -o screensaver screensaver.c -lSDL2 -lm -fopenmp
     ```

   - Construye la imagen Docker:
     ```bash
     docker build -t proyecto_paralela .
     ```

2. **Ejecutar el Contenedor:**
   - Asegúrate de que el servidor X (Xming en Windows o XQuartz en macOS) esté funcionando en tu sistema.
   - Ejecuta el contenedor Docker con el comando adecuado para tu sistema:
     - **Windows:**
       ```powershell
       docker run -it --rm --name mi_contenedor -e DISPLAY=$env:DISPLAY proyecto_paralela /bin/bash
       ```
     - **macOS:**
       ```bash
       docker run -it --rm --name mi_contenedor -e DISPLAY=$DISPLAY proyecto_paralela /bin/bash
       ```

## Proceso de Instalación y Configuración

1. **Instalar y Configurar Xming (Windows):**
   - Descarga e instala Xming.
   - Ejecuta Xming para iniciar el servidor X.
   - Configura la variable `DISPLAY` en PowerShell:
     ```powershell
     $env:DISPLAY="host.docker.internal:0.0"
     ```

2. **Instalar y Configurar XQuartz (macOS):**
   - Descarga e instala XQuartz.
   - Abre XQuartz y configura la opción para permitir conexiones de red en `Preferences > Security`.
   - Configura la variable `DISPLAY` en la terminal:
     ```bash
     export DISPLAY=:0
     ```

3. **Construir y Ejecutar el Contenedor Docker:**
   - Utiliza el `Dockerfile` proporcionado para construir la imagen Docker.
   - Ejecuta el contenedor Docker con el comando adecuado según tu sistema operativo.
