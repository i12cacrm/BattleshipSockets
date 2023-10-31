#ifndef FUNCIONES_H

//includes necesarios
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include<signal.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>


//definicion de macros
#define FUNCIONES_H
#define FILAS 10
#define COLUMNAS 10
#define MSG_SIZE 250
#define MAX_CLIENTS 30
#define MAX_PARTIDAS 10
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
    int num_disparos;
};


//structs
struct cliente
{
    int descriptor;
    //descriptor del socket del cliente
    int inGame;
    //0 si no esta en partida, 1 si esta en partida

    char *username;
    //nombre de usuario

   
    //contraseña de usuario

    int estaLogged;

};


enum estados
{
    vacia,
    semillena,
    llena

};

struct partida
{
    struct cliente jugador1;
    struct cliente jugador2;
    enum estados estado;
    int turno;

    int numdisparos_j1;
    int numdisparos_j2;
    
    
};




//cabecera de funciones

void inicializarVectorClientes(struct cliente clients[]);
void inicializarVectorPartidas(struct partida *partidas);
int buscarCliente(int descriptor, struct cliente clients[], int numClientes);
void salirCliente(int socket, fd_set * readfds, int * numClients, struct cliente clients[]);
int crearPartida(int descriptor, struct cliente *clients, int numClients, struct partida *partidas);
int asignarPartidaVacia(int descriptor, struct cliente *clients, int numClients, struct partida *partidas);
int localizarCliente(int descriptor, int numClients, struct cliente clients[MAX_CLIENTS]);
int localizarPartida(int descriptor, struct partida *partidas);
int comprobarFormato(char * prefijo, char * str);
int registrarUsuario( char *buffer);
int comprobarUsuario(int descriptor,char *buffer, int numClients, struct cliente clients[MAX_CLIENTS]);
int comprobarPassword(int descriptor,char *buffer, int numClients, struct cliente clients[MAX_CLIENTS]);
char* localizarUsuario ( int descriptor, struct cliente clients[MAX_CLIENTS]);
void eliminarPartida(int pos, struct partida *partidas, struct cliente *clients);

void cargarFicheroRegistros(struct cliente *clients);
void escribirFicheroRegistros(char *username, char *password);
int comprobarExistencia (char *username);
int comprobarExistenciaPassword (char *username, char *password);



int calcularTamanoBarco(char tableroBarcos[FILAS][COLUMNAS], int x, int y);
void inicializarBarcos(struct jugador *jugador);
void settearBarco(struct jugador *jugador, int x, int y, size_t size);
void inicializarTableroMapeo(struct jugador *jugador);
void inicializarTableroBarcos(struct jugador *jugador);
void inicializarTableroDisparos(char tablero[FILAS][COLUMNAS]);
int comprobarBarcoAlrededor(char tablero[FILAS][COLUMNAS], int x, int y, int size);
void colocarBarco(char tablero[FILAS][COLUMNAS], int size, int mapeo[FILAS][COLUMNAS], int id);
void colocarBarcosAleatorios(char tablero[FILAS][COLUMNAS],int randomness, int mapeo[FILAS][COLUMNAS]);
void desplegarTablero(char tablero[FILAS][COLUMNAS]);
void desplegarTableroMapeo(int tablero[FILAS][COLUMNAS]);
void tableroACadena(char tablero[FILAS][COLUMNAS], char *cadena);
void cadenaATablero(const char *cadena, struct jugador* jugador);
int procesarDisparo(char tablero[FILAS][COLUMNAS], int x, int y);
void actualizarTableroDisparos(char tablero[FILAS][COLUMNAS], int x, int y, int res);
void actualizarTableroBarcos(char tablero[FILAS][COLUMNAS], int x, int y);
int estaHundido(struct jugador jugador, int x, int y);
int comprobarCoordenada(int x, int y);
int comprobarGanador(struct jugador jugador);
int comprobarTurno(int descriptor, struct partida partida);
















#endif