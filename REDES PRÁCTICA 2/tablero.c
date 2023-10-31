#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define FILAS 10
#define COLUMNAS 10
#define BUFFER_SIZE 250
#define NUMERO_DE_BARCOS 5


struct barco
{
    size_t size;
    int impactos;
    int id;
};



struct jugador
{
    char tableroBarcos[FILAS][COLUMNAS];
    char tableroDisparos[FILAS][COLUMNAS];
    int mapeo[FILAS][COLUMNAS];
    struct barco barcos[NUMERO_DE_BARCOS];
};


/** 
 * ---------BARCOS----------------
*/

int calcularTamanoBarco(char tableroBarcos[FILAS][COLUMNAS], int x, int y)
{
    //printf("x: %d, y: %d\n", x, y);
    int size = 1;
    
    // Buscar horizontalmente hacia la izquierda
    for (int i = y - 1; i >= 0; i--) {
        if (tableroBarcos[x][i] == 'B' || tableroBarcos[x][i] == 'D') {
            size++;
        } else {
            break;
        }
    }
    
    // Buscar horizontalmente hacia la derecha
    for (int i = y + 1; i < COLUMNAS; i++) {
        if (tableroBarcos[x][i] == 'B' || tableroBarcos[x][i] == 'D') {
            size++;
        } else {
            break;
        }
    }
    
    // Buscar verticalmente hacia arriba
    for (int i = x - 1; i >= 0; i--) {
        if (tableroBarcos[i][y] == 'B' || tableroBarcos[i][y] == 'D') {
            size++;
        } else {
            break;
        }
    }
    
    // Buscar verticalmente hacia abajo
    for (int i = x + 1; i < FILAS; i++) {
        if (tableroBarcos[i][y] == 'B' || tableroBarcos[i][y] == 'D') {
            size++;
        } else {
            break;
        }
    }

    return size;
}

void inicializarBarcos(struct jugador *jugador)
{
    struct barco jugador_barcos[] = {{4,0,1}, {3,0,2}, {3,0,3}, {2,0,4}, {2,0,5}};
    
    for( int i = 0; i < NUMERO_DE_BARCOS; i++)
    {
        jugador->barcos[i].size = jugador_barcos[i].size;
        jugador->barcos[i].impactos = jugador_barcos[i].impactos; 
        jugador->barcos[i].id = jugador_barcos[i].id;
        
    }
}


void settearBarco(struct jugador *jugador, int x, int y, size_t size)
{
   // printf("size como parametro: %ld\n", size);
    int id = jugador->mapeo[x][y];
    //printf("id: %d\n", id);
    for (int i = 0; i < NUMERO_DE_BARCOS; i++)
    {
        //printf("jugador->barcos[%d].id: %d, jugador->barcos[%d].size: %ld, jugador->barcos[%d].impactos: %d\n", i,jugador->barcos[i].id, i, jugador->barcos[i].size, i, jugador->barcos[i].impactos);
        if (jugador->barcos[i].id == id && jugador->barcos[i].size == size)
        {
            (jugador->barcos[i].impactos)++;
            //printf(" ESTO ES LO QUE MODIFICOOOOOO: size:%ld, impactos:%d\n", jugador->barcos[i].size, jugador->barcos[i].impactos);
        }
    }
    
}




/**
 * ---------TABLERO----------------
*/


void inicializarTableroMapeo(struct jugador *jugador)
{
    for (int i = 0; i < FILAS; i++)
    {
        for (int j = 0; j < COLUMNAS; j++)
        {
            jugador->mapeo[i][j] = 0;
        }
    }
}

void inicializarTableroBarcos(struct jugador *jugador) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            jugador->tableroBarcos[i][j] = '~';  // Inicializar todas las casillas con agua
        }
    }
}

void inicializarTableroDisparos(char tablero[FILAS][COLUMNAS]) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            tablero[i][j] = '.';  // Inicializar todas las casillas con '.'
        }
    }
}
int comprobarBarcoAlrededor(char tablero[FILAS][COLUMNAS], int x, int y, int size) {
    for (int i = x - 1; i <= x + size; i++) {
        for (int j = y - 1; j <= y + size; j++) {
            if (i >= 0 && i < FILAS && j >= 0 && j < COLUMNAS) {
                if (tablero[i][j] == 'B') {
                    return 0;  // Hay barcos en las casillas adyacentes, no permitido
                }
            }
        }
    }

    return 1;  // No hay barcos en las casillas adyacentes, permitido
}



void colocarBarco(char tablero[FILAS][COLUMNAS], int size, int mapeo[FILAS][COLUMNAS], int id) {
    int x, y, dir;
    do {
        x = 1 + rand() % (FILAS - size + 1);
        y = 1 + rand() % (COLUMNAS - size + 1);
        dir = rand() % 2; // 0 para horizontal, 1 para vertical
    } while (x + (dir == 0 ? size : 0) > FILAS || y + (dir == 1 ? size : 0) > COLUMNAS || !comprobarBarcoAlrededor(tablero, x, y, size));

    for (int i = 0; i < size; i++) {
        tablero[x + (dir == 0 ? i : 0)][y + (dir == 1 ? i : 0)] = 'B';
        mapeo[x + (dir == 0 ? i : 0)][y + (dir == 1 ? i : 0)] = id;
    }
}


void colocarBarcosAleatorios(char tablero[FILAS][COLUMNAS],int randomness, int mapeo[FILAS][COLUMNAS]) {
    srand(time(NULL)+randomness);  // Inicializar la semilla del generador de números aleatorios
    int i = 1;
    colocarBarco(tablero, 4, mapeo, i); // Barco de 4 cuadros
    for (i = 2; i < 4; i++) {
        colocarBarco(tablero, 3,mapeo,i); // 2 Barcos de 3 cuadros
    }
    for (i = 4; i < 6; i++) {
        colocarBarco(tablero, 2,mapeo,i); // 2 Barcos de 2 cuadros
    }
}


void tableroACadena(char tablero[FILAS][COLUMNAS], char *cadena) {
    int indice = 0;
    

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            cadena[indice++] = tablero[i][j];
            if (j < COLUMNAS - 1) {
                cadena[indice++] = ','; // Separador de coordenadas
            }
        }
        cadena[indice++] = ';'; // Separador de filas
    }
    cadena[indice] = '\0'; // Termina la cadena con el carácter nulo
}
void cadenaATablero(const char *cadena, char tablero[FILAS][COLUMNAS]) {
    int fila = 0;
    int columna = 0;
    int indice = 0;
    
    while (cadena[indice] != '\0') {
        if (cadena[indice] == ',') {
            // Ignorar comas
        } else if (cadena[indice] == ';') {
            // Cambiar a la siguiente fila
            fila++;
            columna = 0;
        } else if (cadena[indice] == 'B' || cadena[indice] == '~' || cadena[indice] == 'D'){
            // Almacenar el carácter en la matriz del tablero
            tablero[fila][columna] = cadena[indice];
            columna++;
        }
        indice++;
    }
}


void desplegarTablero(char tablero[FILAS][COLUMNAS]) {
    printf("\n   ");
    for (int j = 0; j < COLUMNAS; j++) {
        printf("%c ", 'A' + j);
    }
    printf("\n");

    for (int i = 0; i < FILAS; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j <COLUMNAS; j++) {
            printf("%c ", tablero[i][j]);
        }
        printf("\n");
    }
}



void desplegarTableroMapeo(int tablero[FILAS][COLUMNAS]) {
    printf("   ");
    for (int j = 0; j < COLUMNAS; j++) {
        printf("%c ", 'A' + j);
    }
    printf("\n");

    for (int i = 0; i < FILAS; i++) {
        printf("%2d ", i + 1);
        for (int j = 0; j <COLUMNAS; j++) {
            printf("%i ", tablero[i][j]);
        }
        printf("\n");
    }
}


/**
 * ---------DISPAROS----------------
*/

int procesarDisparo(char tablero[FILAS][COLUMNAS], int x, int y, char tableroDisparos[FILAS][COLUMNAS]) {
    if ( tableroDisparos[x][y] == 'X' || tablero[x][y] == 'D')
    {
        return 2;  // Ya se había disparado aquí
    }
    else
    {
        if (tablero[x][y] == 'B')
        {
            return 1;  // Tocado
        }
        if (tablero[x][y] == '~')
        {
            return 0;  // Agua
        }
        if ( tableroDisparos[x][y] == 'X' || tablero[x][y] == 'D')
        {
            return 2;  // Ya se había disparado aquí
        }
        
       
    }
    return -1;
    
   
}


//PROPIOS
void actualizarTableroDisparos(char tablero[FILAS][COLUMNAS], int x, int y, int res) {
    if (res == 1)
    {
        tablero[x][y] = 'D';
    }
    else if (res == 0)
    {
        tablero[x][y] = 'x';
    }
    
    
}



void actualizarTableroBarcos(char tablero[FILAS][COLUMNAS], int x, int y) {
   
    tablero[x][y] = 'D';
    
    
}

int estaHundido(struct jugador jugador, int x, int y)
{
    int id = jugador.mapeo[x][y];
    for (int i = 0; i < NUMERO_DE_BARCOS; i++)
    {
        if (jugador.barcos[i].id == id)
        {
            if (jugador.barcos[i].impactos == jugador.barcos[i].size)
            {
                return 1;
            }
        }
    }
    return 0;
}

int comprobarCoordenada(int x, int y)
{
    if (x >= 0 && x < FILAS && y >= 0 && y < COLUMNAS)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 * ---------JUGADOR----------------
*/

int comprobarGanador(struct jugador jugador) {
    for (int i = 0; i < NUMERO_DE_BARCOS; i++) {
        if (jugador.barcos[i].impactos < jugador.barcos[i].size) {
            return 0;  // Todavía no ha ganado
        }
    }

    return 1;  // Ya ha ganado
}


int main() {

    struct jugador jugador1, jugador2;
    int salida = -1;
    int x = 0, y = 0;
    int var = -1;
    int ganador = 0;
    



    // Jugador 1
    inicializarTableroMapeo(&jugador1);
    inicializarTableroBarcos(&jugador1);
    inicializarTableroDisparos(jugador1.tableroDisparos);
    colocarBarcosAleatorios(jugador1.tableroBarcos,0, jugador1.mapeo); // Generar barcos para el jugador 1
    inicializarBarcos(&jugador1);
    


    inicializarTableroMapeo(&jugador2);
    inicializarTableroBarcos(&jugador2);
    inicializarTableroDisparos(jugador2.tableroDisparos);
    colocarBarcosAleatorios(jugador2.tableroBarcos,1,jugador2.mapeo); // Generar barcos para el jugador 2
    inicializarBarcos(&jugador2);
    char cadena[BUFFER_SIZE];
    char buffer[BUFFER_SIZE] = "+Ok. Empieza la partida. ";
    
    


    char letra;
    int turno = 1;
    int numero = 0;
 
    
do
{
    if (turno == 1)
    {
        printf("Jugador %d, es tu turno\n", turno);
        printf("\n----Estos son tus tableros----\n");
        printf("\n----Tablero de barcos para el Jugador %d----\n", turno);
        desplegarTablero(jugador1.tableroBarcos);
        printf("\n----Tablero de disparos para el Jugador %d----\n", turno);
        desplegarTablero(jugador1.tableroDisparos);
        printf("\n----Tablero de mapeo para el Jugador %d----\n", turno);
        desplegarTableroMapeo(jugador1.mapeo);

        x=0;
        y = 0;

        do {
            fflush(stdin);
        printf("Introduce la coordenada para disparar (formato: letra,número): ");
        if (scanf(" %c,%d", &letra, &numero) != 2) {
            printf("Coordenada no válida. Debe tener el formato: letra,número\n");
            while (getchar() != '\n');  // Limpiar el búfer de entrada
        } else {
            y = letra - 'A';
            x = numero - 1;

            if (comprobarCoordenada(x, y) == 0) {
                printf("Coordenada fuera del rango del tablero. Introduce una coordenada válida.\n");
            } else {
                break;  // Coordenadas válidas, salir del bucle
            }
        }
    } while (1);
        
        


        int res = procesarDisparo(jugador2.tableroBarcos, x, y,jugador1.tableroDisparos);
        if ( res == 1 )
        {
            turno = 1;
            var = -1;
            var = calcularTamanoBarco(jugador2.tableroBarcos, x,y);
            //printf("var: %d", var);

            if ( var == -1)
            {
                printf("Error al calcular el tamaño del barco");
            }
            else
            {
                settearBarco(&jugador2, x, y, var);

                if (estaHundido(jugador2, x, y))
                {
                    printf("Hundido\n");
                }
                else
                {
                                       
                    printf("Tocado\n");

                }
                actualizarTableroBarcos(jugador2.tableroBarcos, x, y);

                
            }
        
        }
        else if (res == 0)
        {
            printf("Agua\n");
            turno = 2;


        }
        else if ( res == 2)
        {
            printf("Ya se había disparado aquí\n");
            turno = 2;

        }

        actualizarTableroDisparos(jugador1.tableroDisparos, x, y,res);

    }
    if (turno == 2)
    {
        printf("Jugador %d, es tu turno\n", turno);
        printf("\n----Estos son tus tableros----\n");
        printf("\n----Tablero de barcos para el Jugador %d----\n", turno);
        desplegarTablero(jugador2.tableroBarcos);
        printf("\n----Tablero de disparos para el Jugador %d----\n", turno);
        desplegarTablero(jugador2.tableroDisparos);
        printf("\n----Tablero de mapeo para el Jugador %d----\n", turno);
        desplegarTableroMapeo(jugador2.mapeo);

        x=0;
        y = 0;
       do {
            fflush(stdin);
        printf("Introduce la coordenada para disparar (formato: letra,número): ");
        if (scanf(" %c,%d", &letra, &numero) != 2) {
            printf("Coordenada no válida. Debe tener el formato: letra,número\n");
            while (getchar() != '\n');  // Limpiar el búfer de entrada
        } else {
            y = letra - 'A';
            x = numero - 1;

            if (comprobarCoordenada(x, y) == 0) {
                printf("Coordenada fuera del rango del tablero. Introduce una coordenada válida.\n");
            } else {
                break;  // Coordenadas válidas, salir del bucle
            }
        }
    } while (1);


        int res = procesarDisparo(jugador1.tableroBarcos, x, y, jugador2.tableroDisparos);
        if ( res == 1 )
        {
            int turno = 2;
            var = -1;
            var = calcularTamanoBarco(jugador1.tableroBarcos, x, y);
            //printf("var: %d", var);
            if ( var == -1)
            {
                printf("Error al calcular el tamaño del barco");
            }
            else
            {
                settearBarco(&jugador1, x, y, var);
                if (estaHundido(jugador1, x, y))
                {
                    printf("Hundido\n");
                }
                else
                {
                    
                    printf("Tocado\n");
                }
                actualizarTableroBarcos(jugador1.tableroBarcos, x, y);

                
            }
        
        }
        else if (res == 0)
        {
            printf("Agua\n");
            turno = 1;


        }
        else if ( res == 2)
        {
            printf("Ya se había disparado aquí\n");
            turno = 1;

        }
        

        actualizarTableroDisparos(jugador2.tableroDisparos, x, y,res);

    }
    
    if (comprobarGanador (jugador1))
    {
        ganador = 1;
        break;
    }
    else if (comprobarGanador (jugador2))
    {
        ganador = 2;
        break;
    }
    
    
} while (ganador == 0);
    
    if (ganador == 1)
    {
        printf("Enhorabuena. Ha ganado el Jugador 1\n");
    }
    else if (ganador == 2)
    {
        printf("Enhorabuena. Ha ganado el Jugador 1\n");
    }
    return 0;
}





 
