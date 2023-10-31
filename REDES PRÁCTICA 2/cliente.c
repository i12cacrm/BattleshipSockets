#include "funciones.h"



int main ( )
{
  
	/*---------------------------------------------------- 
		Descriptor del socket y buffer de datos                
	-----------------------------------------------------*/
	int sd;
	struct sockaddr_in sockname;
	char buffer[MSG_SIZE];
	socklen_t len_sockname;
    	fd_set readfds, auxfds;
    	int salida;
    	int fin = 0;
	
	/**--------------------------------------------------
	 * 		Variables para el juego
	*---------------------------------------------------*/
	struct jugador jugador1, jugador2;
    int x = 0, y = 0;
    int var = -1;
    int ganador = 0;
	char letra;
    int turno = 1;
    int numero = 0;
	//char cadena[MSG_SIZE];

    
	/* --------------------------------------------------
		Se abre el socket 
	---------------------------------------------------*/

  	sd = socket (AF_INET, SOCK_STREAM, 0);
	if (sd == -1)
	{
		perror("No se puede abrir el socket cliente\n");
    		exit (1);	
	}

   
    
	/* ------------------------------------------------------------------
		Se rellenan los campos de la estructura con la IP del 
		servidor y el puerto del servicio que solicitamos
	-------------------------------------------------------------------*/
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(2065);
	sockname.sin_addr.s_addr =  inet_addr("127.0.0.1");

	/* ------------------------------------------------------------------
		Se solicita la conexión con el servidor
	-------------------------------------------------------------------*/
	len_sockname = sizeof(sockname);
	
	if (connect(sd, (struct sockaddr *)&sockname, len_sockname) == -1)
	{
		perror ("Error de conexión");
		exit(1);
	}
    
    //Inicializamos las estructuras
    FD_ZERO(&auxfds);
    FD_ZERO(&readfds);
    
    FD_SET(0,&readfds);
    FD_SET(sd,&readfds);

    /*-----------------------------------------------------
	Se inicializan los tableros
	*/
	inicializarTableroMapeo(&jugador1);
    inicializarTableroBarcos(&jugador1);
    inicializarTableroDisparos(jugador1.tableroDisparos);
    //colocarBarcosAleatorios(jugador1.tableroBarcos,0, jugador1.mapeo); // Generar barcos para el jugador 1
    inicializarBarcos(&jugador1);
    


    //inicializarTableroMapeo(&jugador2);
    //inicializarTableroBarcos(&jugador2);
    //inicializarTableroDisparos(jugador2.tableroDisparos);
    //colocarBarcosAleatorios(jugador2.tableroBarcos,1,jugador2.mapeo); // Generar barcos para el jugador 2
    //inicializarBarcos(&jugador2);
	/* ------------------------------------------------------------------
		Se transmite la información
	-------------------------------------------------------------------*/
	do
	{
        auxfds = readfds;
		
        salida = select(sd+1,&auxfds,NULL,NULL,NULL);
        
        //Tengo mensaje desde el servidor
        if(FD_ISSET(sd, &auxfds)){
            

            bzero(buffer,sizeof(buffer));
            recv(sd,buffer,sizeof(buffer),0);
			
			
			printf("\n%s\n",buffer);

			if(comprobarFormato("+Ok. Empezamos partida. ", buffer))
			{
				char *cadena = strstr(buffer, "+Ok. Empezamos partida. ");
				inicializarTableroMapeo(&jugador1);
    			inicializarTableroBarcos(&jugador1);
    			inicializarTableroDisparos(jugador1.tableroDisparos);
    			//colocarBarcosAleatorios(jugador1.tableroBarcos,0, jugador1.mapeo); // Generar barcos para el jugador 1
    			inicializarBarcos(&jugador1);
				

    			if (cadena != NULL) {
    			    char nueva_cadena[MSG_SIZE]; // Debes ajustar el tamaño según tus necesidades
    			    strncpy(nueva_cadena, cadena, buffer - cadena);
			

					//printf("\nTablero: %s\n", nueva_cadena);
					cadenaATablero(cadena, &jugador1);
					printf("\nTABLERO DE BARCOS\n");
					desplegarTablero(jugador1.tableroBarcos);
					printf("\nTABLERO DE DISPAROS\n");
					desplegarTablero(jugador1.tableroDisparos);
			
    			} 

				

			}


			if (comprobarFormato ("+Ok. AGUA: ", buffer))
			{
				x = 0, y=0;
				//printf("aqui entro\n");

				if (sscanf(buffer, "+Ok. AGUA: %c,%d",&letra,&numero) == 2)
				{
					//printf("aqui tambien\n");

					x = numero -1;
					y = letra - 'A';
					//printf("\nEs tu turno\n----------ESTOS SON TUS TABLEROS---------------\n");
					//desplegarTablero(jugador1.tableroBarcos);
					//desplegarTablero(jugador1.tableroDisparos);
					jugador1.tableroDisparos[x][y] = 'x';
					
				}
			}

			if (comprobarFormato ("+Ok. TOCADO: ", buffer))
			{
				x = 0, y=0;


				if (sscanf(buffer, "+Ok. TOCADO: %c,%d",&letra,&numero) == 2)
				{
					x = numero -1;
					y = letra - 'A';
					jugador1.tableroDisparos[x][y] = 'D';

					printf("----------ESTOS SON TUS TABLEROS---------------\n");
					desplegarTablero(jugador1.tableroBarcos);
					printf("\n");
					desplegarTablero(jugador1.tableroDisparos);
					
				}
			}

			if (comprobarFormato ("+Ok. HUNDIDO: ", buffer))
			{
				x = 0, y=0;

				if (sscanf(buffer, "+Ok. HUNDIDO: %c,%d",&letra,&numero ) == 2)
				{
					x = numero -1;
					y = letra - 'A';
					jugador1.tableroDisparos[x][y] = 'D';

					printf("----------ESTOS SON TUS TABLEROS---------------\n");
					desplegarTablero(jugador1.tableroBarcos);
					printf("\n");
					desplegarTablero(jugador1.tableroDisparos);
					
				}
			}
			
			if(comprobarFormato("+Ok.Disparo en: ", buffer))
			{
				if (sscanf(buffer, "+Ok.Disparo en: %c,%d", &letra, &numero) == 2)
				{
					//printf("Entro aqui");
					
					x = numero -1;
					y = letra - 'A';

					int res = procesarDisparo(jugador1.tableroBarcos, x, y);
        			if ( res == 1 )
        			{
        			    var = -1;
        			    var = calcularTamanoBarco(jugador1.tableroBarcos, x, y);
        			    //printf("var: %d", var);
        			    if ( var == -1)
        			    {
        			        printf("Error al calcular el tamaño del barco");
        			    }
        			    else
        			    {        
        			        actualizarTableroBarcos(jugador1.tableroBarcos, x, y);
        			    }

        			}
        			else if (res == 0) //agua
        			{
					printf("----------ESTOS SON TUS TABLEROS---------------\n");
						desplegarTablero(jugador1.tableroBarcos);
						printf("\n");
						desplegarTablero(jugador1.tableroDisparos);
        			}
					
				}

			}

				
            
            if(strcmp(buffer,"Demasiados clientes conectados\n") == 0)
                fin =1;
            
            if(strcmp(buffer,"Desconexión servidor\n") == 0)
                fin =1;
            
        }
        else
        {
            
            //He introducido información por teclado
            if(FD_ISSET(0,&auxfds)){
                bzero(buffer,sizeof(buffer));
                
                fgets(buffer,sizeof(buffer),stdin);
                
                if(strcmp(buffer,"SALIR\n") == 0){
                        fin = 1;
                
                }
                
                send(sd,buffer,sizeof(buffer),0);
                
            }
            
            
        }
        
        
				
    }while(fin == 0);
		
    close(sd);

    return 0;
		
}





















