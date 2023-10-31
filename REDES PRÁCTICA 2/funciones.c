#include "funciones.h"



int buscarCliente(int descriptor, struct cliente clients[], int numClientes)
{

    int i;
    for(i=0; i<numClientes; i++)
        if(clients[i].descriptor == descriptor)
        {
            return i;

        }
    
    return -1;
}


void inicializarVectorClientes(struct cliente *clients)
{

    int i;
    for(i=0; i<MAX_CLIENTS; i++)
    {
        
            clients[i].descriptor = -1;
            clients[i].inGame = 0;
            clients[i].username = NULL;
            //clients[i].password = NULL;
            clients[i].estaLogged = 0;
            clients[i].username = (char *)malloc(MSG_SIZE); 
        //clients[i].password = (char *)malloc(MSG_SIZE);
        
    }
    
}

void inicializarVectorPartidas(struct partida *partidas)
{
    for(int i=0; i<MAX_PARTIDAS; i++)
    {
        partidas[i].jugador1.descriptor = -1;
        partidas[i].jugador2.descriptor = -1;
        partidas[i].estado = vacia; 
        partidas[i].turno = 0;
        partidas[i].numdisparos_j1 = 0;
        partidas[i].numdisparos_j2 = 0;
        

    }
}

void salirCliente(int socket, fd_set * readfds, int * numClientes, struct cliente clients[]){
  
    char buffer[250];
    int j;
    
    close(socket);
    FD_CLR(socket,readfds);
    
    //Re-estructurar el array de clientes
    for (j = 0; j < (*numClientes) - 1; j++)
        if (clients[j].descriptor == socket)
            break;
    for (; j < (*numClientes) - 1; j++)
        (clients[j] = clients[j+1]); 
    
    (*numClientes)--;
    
    bzero(buffer,sizeof(buffer));
    sprintf(buffer,"Desconexión del cliente <%d>",socket);
    
    for(j=0; j<(*numClientes); j++)
        if(clients[j].descriptor != socket)
            send(clients[j].descriptor,buffer,sizeof(buffer),0);


}

int comprobarUsuario(int descriptor,char *buffer, int numClients, struct cliente clients[MAX_CLIENTS])
{
    
    char username[50];
if (sscanf(buffer, "USUARIO %s", username) == 1)
{
    if (comprobarExistencia(username))
    {
        for (int j = 0 ; j<numClients; j++)
        {
           if ( clients[j].descriptor == descriptor && clients[j].estaLogged == 0 && clients[j].inGame == 0)
           {
            for (int k = 0; k<j; k++)
            {
                if (strcmp(clients[k].username, username) == 0)
                {
                    return 2; // El usuario ya esta registrado
                }
            }

               strcpy(clients[j].username, username);
               return 1; // El usuario ya existe   
            }
        }

    } 
    else
    {
        return 0; // El usuario no existe
    }
}
    return -1;   
}




    
int comprobarFormato(char *prefijo, char*str)
{
   
    int i = 0;
    int cont = 0;

    while (prefijo[i] != '\0' && str[i] != '\0' && prefijo[i] == str[i]) {
        cont++;
        i++;
    }

    return (cont == strlen(prefijo));
}


    




int comprobarPassword(int descriptor,char *buffer, int numClients, struct cliente clients[MAX_CLIENTS])
    {
        
        char password[50];        
        if (sscanf(buffer, "PASSWORD %s", password) == 1)
        {
        
                for (int j = 0 ; j<numClients; j++)
                {
                    if ( clients[j].descriptor == descriptor)
                    {
                       if ( comprobarExistenciaPassword(clients[j].username, password))
                       {
                            clients[j].estaLogged = 1;
                            
                            return 1; // El usuario ya existe   
                            
                       }
                    }
                }

            } 
            
            
         
        
        
        return -1;        
    }


/**
 * ----------------------------------------
 * Funciones para el registro de usuarios
 * @return 1 si se ha registrado correctamente
 *  @return 0 si el usuario ya existe
 *  @return -1 si el formato del comando es incorrecto
 * ----------------------------------------
*/
int registrarUsuario(char *buffer) {

    
    char username[50];
    char password[50];

    if (sscanf(buffer, "REGISTRO -u %s -p %s", username, password) == 2)
    {
        if (comprobarExistencia(username))
        {
            return 0; // El usuario ya existe
        } 
        else
        {
            escribirFicheroRegistros(username, password);
            return 1; // El usuario se ha registrado correctamente
        }
    }
                      
            
    return -1;
}




//busca oponente

int crearPartida(int descriptor, struct cliente *clients, int numClients, struct partida *partidas)
{
  

    int pos = localizarCliente(descriptor, numClients, clients);
    if ( pos == -1 )
        return -1;

    for(int i = 0; i<MAX_PARTIDAS; i++)
    {
        if (partidas[i].estado == semillena && partidas[i].jugador1.descriptor != descriptor && clients[pos].inGame == 0)
        {
            clients[pos].inGame = 1;
            partidas[i].jugador2 = clients[pos];
            partidas[i].estado = llena;
            partidas[i].turno = partidas[i].jugador1.descriptor;
            //printf("Turno demasiado ano: %d\n", partidas[i].turno);
            return partidas[i].jugador1.descriptor;
        }
      
        

    }
          
    
    return -1;
   
}


int asignarPartidaVacia(int descriptor, struct cliente *clients, int numClients, struct partida *partidas)
{
    int pos = localizarCliente(descriptor, numClients, clients);
    if ( pos == -1 )
        return -1;

    for(int i = 0; i<MAX_PARTIDAS; i++)
    {
        if(partidas[i].estado == vacia && clients[pos].inGame == 0)
        {
            clients[pos].inGame = 1;
            partidas[i].jugador1 = clients[pos];
            partidas[i].estado = semillena;
            return 0;
        }
    }
    return -1;
}

int localizarCliente(int descriptor, int numClients, struct cliente clients[MAX_CLIENTS])
{
     for (int i = 0; i<MAX_CLIENTS; i++)  
    {
        if ( descriptor == clients[i].descriptor)
        {   
            //clients[i].inGame = 1;
            return i;
        }
    }
    return -1;
    
    
}

int localizarPartida(int descriptor, struct partida *partidas)
{
    for (int i = 0; i < MAX_PARTIDAS; i++)
    {
        if (partidas[i].jugador1.descriptor == descriptor || partidas[i].jugador2.descriptor == descriptor)
        {
            return i;
        }
    }
    return -1;
}


/**
 * ----------------------------------------
 * FICHEROS
 * ----------------------------------------
*/

//void cargarFicheroRegistros(struct cliente *clients)
//{
//    FILE *f;
//    
//    int i = 0;
//    
//    f = fopen("usuarios.txt", "r");
//    if (f == NULL)
//    {
//        printf("Error al abrir el fichero");
//        exit(-1);
//    }
//    
//    char linea[MSG_SIZE]; // Tamaño suficiente para una línea en tu archivo
//    while (fgets(linea, sizeof(linea), f))
//    {
//        // Asignar memoria para username y password
//        //clients[i].username = malloc(sizeof(char*)*MSG_SIZE); // Asumiendo una longitud máxima de 100 caracteres
//        //clients[i].password = malloc(sizeof(char*)*MSG_SIZE);
//
//        if (sscanf(linea, "%s %s\n", clients[i].username, clients[i].password) == 2)
//        {
//            i++;
//        }
//    }
//
//    fclose(f);
//}

void escribirFicheroRegistros(char *username, char *password)

{
    FILE *f;
    f =fopen("usuarios.txt","a");
    if ( f== NULL)
    {
        printf("Error al abrir el fichero\n");
        exit(EXIT_FAILURE);
    }

  
        fprintf(f,"%s %s\n",username,password);
        
    
    
    fclose(f);
}


int comprobarExistencia (char *username)
{
    char username2[MSG_SIZE];
    char password[MSG_SIZE];
    FILE *f;
    f =fopen("usuarios.txt","r");
    if ( f== NULL)
    {
        printf("Error al abrir el fichero\n");
        exit(EXIT_FAILURE);
    }

    char linea[MSG_SIZE];
  
    while (fgets(linea, sizeof(linea), f))
    {
        
        
        if (sscanf(linea, "%s %s", username2, password) == 2)
        {
            if (strcmp(username, username2) == 0)
            {
                fclose(f);
                return 1;
            }
        }
    }

        
    fclose(f);   
    return 0;
}




int comprobarExistenciaPassword (char *username, char *password)
{
    char username2[MSG_SIZE];
    char password2[MSG_SIZE];
    FILE *f;
    f =fopen("usuarios.txt","r");
    if ( f== NULL)
    {
        printf("Error al abrir el fichero\n");
        exit(EXIT_FAILURE);
    }

    char linea[MSG_SIZE];
  
    while (fgets(linea, sizeof(linea), f))
    {
        
        
        if (sscanf(linea, "%s %s", username2, password2) == 2)
        {
            if (strcmp(password, password2) == 0 && strcmp(username, username2) == 0)
            {
                fclose(f);
                return 1;
            }
        }
    }

        
    fclose(f);   
    return 0;
}


/**
 * ----------------------------------------
 * -                                      -
 * -    FUNCIONES PARA EL JUEGO           -
 * -                                      -   
 * ----------------------------------------
*/


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
            jugador->tableroBarcos[i][j] = 'A';  // Inicializar todas las casillas con agua
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

void desplegarTablero(char tablero[FILAS][COLUMNAS]) {
    printf("   ");
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


void cadenaATablero(const char *cadena, struct jugador *jugador) {
    int fila = 0;
    int columna = 0;
    int indice = 0;
    
    //printf("cadena cadenon: %s\n", cadena);
    while (cadena[indice] != '\0') {
        if (cadena[indice] == ',') {
            // Ignorar comas
        } else if (cadena[indice] == ';') {
            // Cambiar a la siguiente fila
            fila++;
            columna = 0;
        } else if (cadena[indice] == 'B' || cadena[indice] == 'A' || cadena[indice] == 'D'){
            // Almacenar el carácter en la matriz del tablero
            jugador->tableroBarcos[fila][columna] = cadena[indice];
            columna++;
        }
        indice++;
    }

    for (int i= 0; i<FILAS; i++)
    {
        for (int j = 0; j<COLUMNAS; j++)
        {
            if (jugador->tableroBarcos[i][j] == 'B')
            {
                jugador->mapeo[i][j] = calcularTamanoBarco(jugador->tableroBarcos, i, j);
            }
            
        }
        
    }
    
}
/**
 * ---------DISPAROS----------------
*/

int procesarDisparo(char tablero[FILAS][COLUMNAS], int x, int y) {
   
        if (tablero[x][y] == 'B')
        {
            return 1;  // Tocado
        }
        if (tablero[x][y] == 'A')
        {
            return 0;  // Agua
        }
        if ( tablero[x][y] == 'D')
        {
            return 2;  // Ya se había disparado aquí
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
            return 0;  // Todavía no ha perdido
        }
    }

    return 1;  // Ya ha perdido
}

int comprobarTurno(int descriptor, struct partida partida)
{
    
        if (partida.turno == descriptor)
        {
            return 1;
        }
        
    return 0;
    
}



char* localizarUsuario ( int descriptor, struct cliente clients[MAX_CLIENTS])
{
    for ( int i = 0; i<MAX_CLIENTS; i++)
    {
            if (descriptor == clients[i].descriptor)
            {
                return clients[i].username;
            }
    }

}


void eliminarPartida(int pos, struct partida *partidas, struct cliente *clients)
{

    
    
    for ( int i = 0; i< MAX_CLIENTS; i++)
    {
        if ( partidas[pos].jugador1.descriptor == clients[i].descriptor )
        {
            clients[i].inGame = 0;

        }
         if ( partidas[pos].jugador2.descriptor == clients[i].descriptor )
        {
            clients[i].inGame = 0;

        }
        
    }  
       
    partidas[pos].jugador1.descriptor = -1;
    partidas[pos].jugador2.descriptor = -1;
    partidas[pos].estado = vacia; 
    partidas[pos].turno = 0;
    partidas[pos].numdisparos_j1 = 0;
    partidas[pos].numdisparos_j2 = 0;

   



            
}

        

int salirDePartida( int descriptor, struct partida *partidas)
{
    for ( int i = 0; i< MAX_PARTIDAS; i++)
    {
        if ( partidas[i].jugador1.descriptor == descriptor || partidas[i].jugador2.descriptor == descriptor)
        {
            if ( partidas[i].jugador1.descriptor == descriptor)
            {
                partidas[i].jugador1.descriptor = -1;
                partidas[i].jugador1.inGame = 0;
            }
            else
            {
                partidas[i].jugador2.descriptor = -1;
                partidas[i].jugador2.inGame = 0;
            }
            if ( partidas[i].estado == llena )
            {
                partidas[i].estado = semillena;
            }
            else
            {
                partidas[i].estado = vacia;
            
            }

        }
    }
    return -1;
}