#include "funciones.h"



int main()
{
    int descriptor,clientDescriptor;

    //vector de clientes
    struct cliente clients[MAX_CLIENTS];

    //vector partida

    
    
    struct partida partidas[MAX_PARTIDAS];
    
    int numClients = 0;

    int usuarioLoggeado = 0;

    char buffer[MSG_SIZE];
    
    struct sockaddr_in sockname, from;
    socklen_t socknameLength = sizeof(sockname);
    socklen_t fromLength = sizeof(from);


    int usuario_valido, password_valida;
        
    int salida;
    fd_set readfds;
    fd_set auxfds;

    int on, ret;

    int i, j,z;

    int recibidos;

/**
 * --------------------------------------------------
 *      Se inicializan los vectores y se carga el fichero de registros
 * --------------------------------------------------
*/
    inicializarVectorClientes(clients);

    //cargarFicheroRegistros(clients);
    
    inicializarVectorPartidas(partidas);

    //for (int i = 0; i < MAX_CLIENTS; i++)
    //{
    //    printf("Cliente %d: %d\n", i, clients[i].descriptor);
    //    printf("Cliente %d: %s\n", i, clients[i].username);
    //    printf("Cliente %d: %s\n", i, clients[i].password);
    //}




/*--------------------------------------------------
 *      JUEGO
 * --------------------------------------------------
*/
    struct jugador jugador1, jugador2;
int x = 0, y = 0;
int var = -1;
int ganador = 0;
char letra;
int turno = 1;
int numero = 0;
char cadena[MSG_SIZE];


// Jugador 1
inicializarTableroMapeo(&jugador1);
inicializarTableroBarcos(&jugador1);
colocarBarcosAleatorios(jugador1.tableroBarcos,0, jugador1.mapeo); // Generar barcos para el jugador 1
inicializarBarcos(&jugador1);
jugador1.num_disparos = 0;


//Jugador 2

inicializarTableroMapeo(&jugador2);
inicializarTableroBarcos(&jugador2);
colocarBarcosAleatorios(jugador2.tableroBarcos,1,jugador2.mapeo); // Generar barcos para el jugador 2
inicializarBarcos(&jugador2);
jugador2.num_disparos = 0;




/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/
    descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if(descriptor == -1)
    {
        fprintf(stderr, "No se puede abrir el socket servidor: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    /*
    --------------------------------------------------
    Para reutilizar el puerto
    */

    on = 1;
    ret = setsockopt( descriptor, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));


    /*
    --------------------------------------------------
        Asignamos la dirección al socket
    --------------------------------------------------
    
    
    */
   sockname.sin_addr.s_addr = INADDR_ANY;
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(2065);

    if(bind(descriptor, (struct sockaddr *) &sockname,socknameLength) == -1)
    {
        fprintf(stderr, "Error en la operación bind: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    //Establecemos el socket en modo escucha
    if(listen(descriptor, 1) == -1)
    {
        fprintf(stderr, "Error en la operación de listen: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    printf("El servidor está esperando conexiones...\n");

    FD_ZERO(&readfds);
    FD_ZERO(&auxfds);
    FD_SET(0, &readfds);
    FD_SET(descriptor, &readfds);

    while(1)
    {
        
        auxfds = readfds;
        salida = select(FD_SETSIZE, &auxfds, NULL, NULL, NULL);

        if(salida>0)        
        {
            for(i = 0; i<FD_SETSIZE; i++)
            {
                if(FD_ISSET(i,&auxfds))
                {

                    if(i == descriptor)
                    {
                        //Aceptamos la conexión
                        if((clientDescriptor = accept(descriptor ,(struct sockaddr* )&from, &fromLength)) == -1)
                        {
                            fprintf(stderr, "Error en la operación Accept: %s\n", strerror(errno));
                        }

                        else
                        {
                            if(numClients < MAX_CLIENTS)
                            {
                                clients[numClients].descriptor = clientDescriptor;
                                numClients++;
                                FD_SET(clientDescriptor, &readfds);

                                strcpy(buffer, "+Ok. Usuario conectado");

                                send(clientDescriptor, buffer, sizeof(buffer), 0);
                                

                                for(j = 0; j<(numClients-1);j++)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    sprintf(buffer, "Nuevo Cliente conectado en <%d>\n", clientDescriptor);
                                    send(clients[j].descriptor, buffer, sizeof(buffer), 0);
                                }
                                printf("Nuevo Cliente conectado en <%d>\n", clientDescriptor);

                                
                            }
                            else
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "-Err. Demasiados clientes conectados.\n");
                                send(clientDescriptor, buffer, sizeof(buffer), 0);
                                close(clientDescriptor);
                            }
                        }

                    }
                    else if(i == 0)
                    {
                        bzero(buffer, sizeof(buffer));
                        fgets(buffer, sizeof(buffer), stdin);



                        if(strcmp(buffer, "SALIR\n") == 0)
                        {
                            for(int j = 0; j<numClients; j++)
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "Desconexión servidor\n");
                                send(clients[j].descriptor, buffer, sizeof(buffer), 0);
                                close(clients[j].descriptor);
                                FD_CLR(clients[j].descriptor, &readfds);
                            }
                        
                            //escribirFicheroRegistros(clients, numClients);
                            printf("Servidor cerrado\n");
                            close(descriptor);
                            exit(-1);
                        }
                    }


                    
                    else
                    {
                        
                        //buscaremos al cliente que ha enviado el mensaje
                        if (buscarCliente(i, clients, numClients) == -1)
                        {
                            printf("Error al buscar al cliente\n");
                            exit(-1);
                        }
                        
                        bzero(buffer, sizeof(buffer));
                        if((recibidos = recv(i, buffer, sizeof(buffer), 0)) > 0)
                        {
                            if(strcmp(buffer, "SALIR\n") == 0)
                            {
                                //escribirFicheroRegistros(clients, numClients);

                                int pos = localizarCliente(i, numClients, clients);
                                if ( clients[pos].inGame == 1)
                                {
                                    int pos2 = localizarPartida(i, partidas);
                                    

                                  if (partidas[pos2].estado == llena && partidas[pos2].jugador1.descriptor == i)
                                  {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Tu oponente ha abandonado la partida\n");
                                        send(partidas[pos2].jugador2.descriptor, buffer, sizeof(buffer), 0);

                                 }
                               else if (partidas[pos2].estado == llena &&  partidas[pos2].jugador2.descriptor == i)
                                    {

                                      bzero(buffer, sizeof(buffer));
                                     strcpy(buffer, "+Ok. Tu oponente ha abandonado la partida\n");
                                        send(partidas[pos2].jugador1.descriptor, buffer, sizeof(buffer), 0);

                                  }
                                
                                    eliminarPartida(pos2, partidas,clients);

                                                              

                                }
                                salirCliente(i, &readfds, &numClients, clients);

                            }
                            else if(comprobarFormato("REGISTRO ", buffer))
                            {
                                int res = registrarUsuario(buffer);
                                if ( res == 1)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Usuario registrado\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else if ( res == 0)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. El usuario ya existe\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else if ( res == -1)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Error en el comando de registro\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                               

                            }  
                           
    
                            else if(comprobarFormato("USUARIO ", buffer))
                            {
                               
                                usuario_valido = comprobarUsuario(i, buffer,numClients, clients);

                                if(usuario_valido == 0)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usuario incorrecto o no registrado. \n");
                                    send(i, buffer, sizeof(buffer), 0);

                                }
                                else if (usuario_valido == 1 )
                                {
                                    
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "+Ok. Usuario correcto. Introduzca la contraseña:\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                    
                                }

                                else if (usuario_valido == 2)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usuario ya loggeado.\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Usuario en el comando usuario.\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                            
                            }
                            else if(comprobarFormato("PASSWORD ", buffer))
                            {
                                if ( usuario_valido == 1)
                                {
                                     password_valida = comprobarPassword(i,buffer,numClients,clients);
                                        
                                        if(password_valida == 1)
                                        {
                                            int pos = localizarCliente(i, numClients, clients);
                                            if (pos == -1)
                                            {
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "-Err. Error al localizar al cliente\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                            else
                                            {
                                                clients[pos].estaLogged = 1;
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "+Ok. Usuario validado\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            }
                                            
                                            
                                        }
                                        else
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Error en la validacion.\n");
                                            send(i, buffer, sizeof(buffer), 0);


                                        }
                                }
                                
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Debes introducir el usuario antes de la contraseña.\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                                
                                       
                            }
                            
                            else if (comprobarFormato("INICIAR-PARTIDA\n", buffer))
                            {
                                
                                if ( clients[localizarCliente(i, numClients, clients)].estaLogged == 1 && clients[localizarCliente(i, numClients, clients)].descriptor == i)
                                {
                                    // Jugador 1
                                    inicializarTableroMapeo(&jugador1);
                                    inicializarTableroBarcos(&jugador1);
                                    colocarBarcosAleatorios(jugador1.tableroBarcos,0, jugador1.mapeo); // Generar barcos para el jugador 1
                                    inicializarBarcos(&jugador1);
                                    jugador1.num_disparos = 0;


                                    //Jugador 2

                                    inicializarTableroMapeo(&jugador2);
                                    inicializarTableroBarcos(&jugador2);
                                    colocarBarcosAleatorios(jugador2.tableroBarcos,1,jugador2.mapeo); // Generar barcos para el jugador 2
                                    inicializarBarcos(&jugador2);
                                    jugador2.num_disparos = 0;
                                    int pos=crearPartida(i, clients, numClients, partidas);
                                    if (pos > 0)
                                    {
                                    
                                        bzero(buffer, sizeof(buffer));
                                        sprintf(buffer, "+Ok. Encontrado oponente con socket %d\n", i);
                                        send(pos, buffer, sizeof(buffer), 0);


                                        // Envio de tableros a los jugadores
                                        tableroACadena(jugador1.tableroBarcos, cadena);
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Empezamos partida. ");
                                        strcat(buffer, cadena);
                                        send(pos, buffer, sizeof(buffer), 0);

                                        tableroACadena(jugador2.tableroBarcos, cadena);
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Empezamos partida. ");
                                        strcat(buffer, cadena);
                                        send(i, buffer, sizeof(buffer), 0);

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Turno de partida: ");
                                        send(pos, buffer, sizeof(buffer), 0);
                                        //printf("En partida: %d\n", clients[localizarCliente(i, numClients, clients)].inGame);
                                        //printf("En partida: %d\n", clients[localizarCliente(pos, numClients, clients)].inGame);


                                        //for(int i= 0; i<MAX_PARTIDAS; i++)
                                        //{
                                        //    printf("Partida %d: %d\n", i, partidas[i].jugador1.descriptor);
                                        //    printf("Partida %d: %d\n", i, partidas[i].jugador2.descriptor);
                                        //    printf("Partida %d: %d\n", i, partidas[i].estado);
                                        //    printf("Partida %d: %d\n", i, partidas[i].turno);
                                        //}
                                        //  
                                        //printf("Turno ano: %d\n", partidas[localizarPartida(i, partidas, numPartidas)].turno);



                                    } 
                                    else
                                    {
                                        int res = asignarPartidaVacia(i, clients, numClients, partidas);

                                        if ( res == 0 )
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Ok. Esperando jugadores\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
                                        else
                                        {
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Ha habido un error al iniciar partida\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        }
    
                                    }
                                }
                                else
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. Debes iniciar sesión para jugar\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                
                                
                            }  
                            else if (comprobarFormato("DISPARO ",buffer))
                            {
                                if ( clients[localizarCliente(i, numClients, clients)].inGame == 0)
                                {
                                    bzero(buffer, sizeof(buffer));
                                    strcpy(buffer, "-Err. No estás en partida\n");
                                    send(i, buffer, sizeof(buffer), 0);
                                }
                                else
                                {
                                    struct partida partida = partidas[localizarPartida(i, partidas)];
                                    //printf("Turno: %d\n", partida.turno);
                                    if (comprobarTurno(i,partida))
                                    {
                                        x = 0;
                                        y = 0;


                                        if (sscanf(buffer,"DISPARO %c,%d", &letra, &numero) != 2) {
                                            bzero(buffer, sizeof(buffer));
                                            strcpy(buffer, "-Err. Formato incorrecto. Debe tener el formato: letra,número\n");
                                            send(i, buffer, sizeof(buffer), 0);
                                        } 
                                        else
                                        {
                                            y = letra - 'A';
                                            x = numero - 1;
                                            if (comprobarCoordenada(x, y) == 0) {
                                                bzero(buffer, sizeof(buffer));
                                                strcpy(buffer, "-Err. Coordenada incorrecta. Debe estar entre A1 y J10\n");
                                                send(i, buffer, sizeof(buffer), 0);
                                            } 
                                            else {
                                                printf("\n-----PARTIDA %d-------\n",localizarPartida(i, partidas));
                                                char username_ganador[50];

                                                if(partida.turno == partida.jugador1.descriptor)
                                                {
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok.Disparo en: %c,%d\n", letra, numero);
                                                    send(partida.jugador2.descriptor, buffer, sizeof(buffer), 0);

                                                    jugador1.num_disparos++;

                                                    printf("Turno jugador 1\n");
                                                    printf("\n----Tablero de barcos para el Jugador 1----\n");
                                                    desplegarTablero(jugador1.tableroBarcos);

                                                    bzero(buffer, sizeof(buffer));                                                    

                                                    int res = procesarDisparo(jugador2.tableroBarcos, x, y);
                                                    if ( res == 1 )
                                                    {

                                                        partidas[localizarPartida(i, partidas)].turno = partida.jugador1.descriptor;
                                                        var = -1;
                                                        var = calcularTamanoBarco(jugador2.tableroBarcos, x,y);
                                                        //printf("var: %d", var);

                                                        if ( var == -1)
                                                        {
                                                            bzero(buffer, sizeof(buffer));
                                                            strcpy(buffer, "-Err. Error al calcular el tamaño del barco\n");
                                                            send(i, buffer, sizeof(buffer), 0);
                                                           // printf("Error al calcular el tamaño del barco");
                                                        }
                                                        else
                                                        {
                                                            settearBarco(&jugador2, x, y, var);

                                                            if (estaHundido(jugador2, x, y))
                                                            {
                                                                bzero(buffer, sizeof(buffer));
                                                                sprintf(buffer, "+Ok. HUNDIDO: %c,%d\n",letra, numero);
                                                                send(i, buffer, sizeof(buffer), 0);

                                                                
                                                                //printf("Hundido\n");
                                                            }
                                                            else
                                                            {
                                                                bzero(buffer, sizeof(buffer));
                                                                sprintf(buffer, "+Ok. TOCADO: %c,%d\n",letra, numero);
                                                                send(i, buffer, sizeof(buffer), 0);

                                                            }
                                                            bzero(buffer, sizeof(buffer));
                                                            sprintf(buffer, "+Ok. Turno de partida:\n");
                                                            send(i, buffer, sizeof(buffer), 0);
                                                            actualizarTableroBarcos(jugador2.tableroBarcos, x, y);


                                                        }

                                                    }
                                                   
                                                    else if ( res == 2)
                                                    {
                                                        bzero(buffer, sizeof(buffer));
                                                        strcpy(buffer, "-Err. Ya se había disparado en este barco. vuelve a disparar.\n");
                                                        send(i, buffer, sizeof(buffer), 0);
                                    
                                                        //printf("Ya se había disparado aquí\n");
                                                        partidas[localizarPartida(i, partidas)].turno = partida.jugador1.descriptor;
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Turno de partida:\n");
                                                        send(i, buffer, sizeof(buffer), 0);

                                                    }
                                                     else if (res == 0)
                                                    {
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. AGUA: %c,%d\n",letra, numero);
                                                        send(i, buffer, sizeof(buffer), 0);
                                                        //printf("Agua\n");
                                                        partidas[localizarPartida(i, partidas)].turno = partida.jugador2.descriptor;

                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Turno de partida:\n");
                                                        send(partida.jugador2.descriptor, buffer, sizeof(buffer), 0);


                                                    }
                                                  
                                                                                                        
                                                }
                                                if (partida.turno == partida.jugador2.descriptor)
                                                {
                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok.Disparo en: %c,%d\n", letra, numero);
                                                    send(partida.jugador1.descriptor, buffer, sizeof(buffer), 0);

                                                    jugador2.num_disparos++;

                                                    printf("Turno jugador 2\n");
                                                    printf("\n----Tablero de barcos para el Jugador 2----\n");
                                                    desplegarTablero(jugador2.tableroBarcos);


                                                    int res = procesarDisparo(jugador1.tableroBarcos, x, y);
                                                    if ( res == 1 )
                                                    {
                                                        partidas[localizarPartida(i, partidas)].turno = partida.jugador2.descriptor;
                                                        var = -1;
                                                        var = calcularTamanoBarco(jugador1.tableroBarcos, x,y);
                                                        //printf("var: %d", var);

                                                        if ( var == -1)
                                                        {
                                                            bzero(buffer, sizeof(buffer));
                                                            strcpy(buffer, "-Err. Error al calcular el tamaño del barco\n");
                                                            send(i, buffer, sizeof(buffer), 0);
                                                           // printf("Error al calcular el tamaño del barco");
                                                        }
                                                        else
                                                        {
                                                            settearBarco(&jugador1, x, y, var);

                                                            if (estaHundido(jugador1, x, y))
                                                            {
                                                                bzero(buffer, sizeof(buffer));
                                                                sprintf(buffer, "+Ok. HUNDIDO: %c,%d\n",letra, numero);
                                                                send(i, buffer, sizeof(buffer), 0);
                                                                //printf("Hundido\n");
                                                            }
                                                            else
                                                            {
                                                                bzero(buffer, sizeof(buffer));
                                                                sprintf(buffer, "+Ok. TOCADO: %c,%d\n",letra, numero);
                                                                send(i, buffer, sizeof(buffer), 0);

                                                            }
                                                            actualizarTableroBarcos(jugador1.tableroBarcos, x, y);

                                                            bzero(buffer, sizeof(buffer));
                                                            sprintf(buffer, "+Ok. Turno de partida:\n");
                                                            send(i, buffer, sizeof(buffer), 0);


                                                        }

                                                    }
                                                   
                                                    else if ( res == 2)
                                                    {
                                                        bzero(buffer, sizeof(buffer));
                                                        strcpy(buffer, "-Err. Ya habías disparado en este barco. Vuelve a disparar\n");
                                                        send(i, buffer, sizeof(buffer), 0);
                                                        //printf("Ya se había disparado aquí\n");
                                                        partidas[localizarPartida(i, partidas)].turno = partida.jugador2.descriptor;
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Turno de partida:\n");
                                                        send(i, buffer, sizeof(buffer), 0);

                                                    }
                                                    
                                                    else if (res == 0)
                                                    {
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. AGUA: %c,%d\n",letra, numero);
                                                        send(i, buffer, sizeof(buffer), 0);
                                                        //printf("Agua\n");
                                                        partidas[localizarPartida(i,  partidas)].turno = partida.jugador1.descriptor;
                                                        
                                                        bzero(buffer, sizeof(buffer));
                                                        sprintf(buffer, "+Ok. Turno de partida:\n");
                                                        send(partida.jugador1.descriptor, buffer, sizeof(buffer), 0);

                                                    }
                                                    

                                                    
                                                }
                                                if (comprobarGanador (jugador1))
                                                {
                                                    strcpy(username_ganador, localizarUsuario(partida.jugador2.descriptor, clients));

                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. %s ha ganado, numero de disparos %d", username_ganador, jugador2.num_disparos);
                                                    send(partida.jugador1.descriptor, buffer, sizeof(buffer), 0);
                                                    send(partida.jugador2.descriptor, buffer, sizeof(buffer), 0);
                                                    int pos1 = localizarPartida(partida.jugador2.descriptor, partidas);
                                                    eliminarPartida(pos1, partidas,clients);
                                            
                                                   

                                                    

                                                    
                                                }
                                                else if (comprobarGanador (jugador2))
                                                {
                                                    
                                                    strcpy(username_ganador, localizarUsuario(partida.jugador1.descriptor, clients));

                                                    bzero(buffer, sizeof(buffer));
                                                    sprintf(buffer, "+Ok. %s ha ganado, numero de disparos %d", username_ganador, jugador1.num_disparos);
                                                    send(partida.jugador1.descriptor, buffer, sizeof(buffer), 0);
                                                    send(partida.jugador2.descriptor, buffer, sizeof(buffer), 0);
                                                    //eliminarPartida
                                                    int pos2 = localizarPartida(partida.jugador1.descriptor, partidas);
                                                    eliminarPartida(pos2, partidas,clients);  
                                                    
                                                    

                                                    //for ( int z= 0; z< MAX_PARTIDAS; z++)
                                                    //{
                                                    //    printf("Partida %d: %d\n", z, partidas[z].jugador1.descriptor);
                                                    //    printf("Partida %d: %d\n", z, partidas[z].jugador2.descriptor);
                                                    //    printf("Partida %d: %d\n", z, partidas[z].estado);
                                                    //    printf("Partida %d: %d\n", z, partidas[z].turno);
                                                    //}
                                                    
                                                    
                                                    
                                                    
                                                }
                                                
                                            }
                                        }
                                    }
                                    else
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "-Err. Debe esperar su turno\n");
                                        send(i, buffer, sizeof(buffer), 0);
                                    }
                                }
                             
                            }
                            else
                            {
                                bzero(buffer, sizeof(buffer));
                                strcpy(buffer, "-Err. Comando no reconocido\n");
                                send(i, buffer, sizeof(buffer), 0);
                            }              
                        }
                       
                        if(recibidos== 0)
                        {
                            printf("El socket %d, ha introducido ctrl+c\n", i);
                            //Eliminar ese socket
                            int pos = localizarCliente(i, numClients, clients);
                                if ( clients[pos].inGame == 1)
                                {
                                    int pos2 = localizarPartida(i, partidas);
                                    

                                    if (partidas[pos2].estado == llena && partidas[pos2].jugador1.descriptor == i)
                                    {
                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Tu oponente ha abandonado la partida\n");
                                        send(partidas[pos2].jugador2.descriptor, buffer, sizeof(buffer), 0);

                                    }
                                    else if (partidas[pos2].estado == llena &&  partidas[pos2].jugador2.descriptor == i)
                                    {

                                        bzero(buffer, sizeof(buffer));
                                        strcpy(buffer, "+Ok. Tu oponente ha abandonado la partida\n");
                                        send(partidas[pos2].jugador1.descriptor, buffer, sizeof(buffer), 0);

                                    }
                                
                                    eliminarPartida(pos2, partidas,clients);
                                                              
                                }
                            salirCliente(i,&readfds,&numClients,clients);
                            
                        }


                    }

                }
            }  
        }


        
    }

    //escribirFicheroRegistros(clients, numClients);
    close(descriptor);
    return 0;
}



    
    