## Instructions for environment setup

First we must build the docker image using the `Dockerfile` in the root directory
```bash
docker build -t proyecto_paralela2 .
```

Then we use this to run a container with the image we just did
```bash
docker run -it --rm --name mi_contenedor \
-v .:/usr/src/ proyecto_paralela2 /bin/bash
```

NOTE: If you are running this on windows we recomend using WSL or at least replacing the '.' on the bind mount (i.e. `-v .:/usr/src/`) for the current directory.

Once in the container, make sure you are on the working directory `/usr/src/`. 

To compile the program we use: 
```bash
gcc -o test.o test.c `pkg-config --cflags --libs cairo` -lm -fopenmp
```

Then to run it we call the executable and along it the number of curves we want to draw, in this example is 1000. 

```bash
./test.o 100 '<numero de >'
```
