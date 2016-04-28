#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int
main (int argc, char *argv[])
{
	char *servidor_ip;
	char *servidor_puerto;
	char *mensaje, respuesta[1024];
	struct sockaddr_in direccion;
	int s;
	int option=0;
	int n, enviados, recibidos;
	char peticion[1024];
	char edificio[10];

	/* Comprobar los argumentos */
	if (argc !=  4)
	{
		fprintf(stderr, "Error. Debe indicar la direccion del servidor (IP y Puerto) y el mensaje a enviar\r\n");
		fprintf(stderr, "Sintaxis: %s <ip> <puerto> <mensaje>\n\r", argv[0]);
		fprintf(stderr, "Ejemplo : %s 192.168.6.7 8574 \"Esto es un mensaje\"\n\r", argv[0]);
		return 1;
	}
	
	/* Tomar los argumentos */		
	servidor_ip = argv[1];
	servidor_puerto = argv[2];
	mensaje = argv[3];
	
	do{
		printf("Escoja una opción:\n");
			printf("1.-Listado de edificios de la Universidad de Alicante.\n");
			printf("2.-Numero de estacias por edificio.\n");
			printf("3.-Listado de edificions con estancias sin ocupantes asignados.\n");
			printf("4.-Número de estancias y ocupantes por edificio.\n");
			scanf("%d",&option);
	}while(option<=0||option>4);
	if(option==1){
		strcpy(peticion, "GET /api/pub/edificio/all/items HTTP/1.1"");
	}else if(option==2){
		strcpy(peticion, "GET /api/pub/estancia/edificio/");
		printf("Elige el edificio (4 numeros):\n");
		scanf("%s",edificio);
		strcat(peticion, edificio);
		strcat(peticion, "/items HTTP/1.1");
	}
	else if(option==3){
		strcpy(cad,"GET /api/pub/edificio/despachos_vacios/items HTTP/1.1");
	}
	else if(option==4){
		strcpy(cad,"GET /api/agregados/seo/edificio/all/items HTTP/1.1");
	}
}