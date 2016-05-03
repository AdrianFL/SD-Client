#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <jansson.h>
#include <stdarg.h>
#include <netdb.h>

int
main (int argc, char *argv[])
{
	char *servidor_ip;
	char *servidor_puerto;
	char *mensaje, respuesta[10000];
	struct sockaddr_in direccion;
	int s, i;
	int option=0;
	int n, enviados, recibidos;
	char peticion[1024], fichero_final[10000], aux[10000];
	char edificio[10];
	struct hostent *new;
	struct in_addr **addr_list;
	json_t *data;
    json_error_t error;
	

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
		strcpy(peticion, "GET /api/pub/edificio/all/items HTTP/1.1");
	}else if(option==2){
		strcpy(peticion, "GET /api/pub/estancia/edificio/");
		printf("Elige el edificio (4 numeros):\n");
		scanf("%s",edificio);
		strcat(peticion, edificio);
		strcat(peticion, "/items HTTP/1.1");
	}
	else if(option==3){
		strcpy(peticion,"GET /api/pub/edificio/despachos_vacios/items HTTP/1.1");
	}
	else if(option==4){
		strcpy(peticion,"GET /api/agregados/seo/edificio/all/items HTTP/1.1");
	}
	
	
	servidor_ip=argv[1];
	servidor_puerto=argv[2];
	mensaje=argv[3];
	
	//obtener la direccion ip
	new=gethostbyname(servidor_ip);
	if(new==NULL){
		printf("Error de escritura en el host. \n");
		return 1;
	}
	addr_list=(struct in_addr **)new->h_addr_list;
	servidor_ip=inet_ntoa(*addr_list[0]);
	printf("Direccion del servidor: %s", servidor_ip);
	printf("\n\rEnviar mensaje \"%s\" a %s:%s...\n\r\n\r", mensaje, argv[1], servidor_puerto);
	
	
	
	/**** Paso 1: Abrir el socket ****/

	s = socket(AF_INET, SOCK_STREAM, 0); /* creo el socket */
	if (s == -1)
	{
		fprintf(stderr, "Error. No se puede abrir el socket\n\r");
		return 1;
	}
	printf("Socket abierto\n\r");

	/**** Paso 2: Conectar al servidor ****/		

	/* Cargar la dirección */
	direccion.sin_family = AF_INET; /* socket familia INET */
	direccion.sin_addr.s_addr = inet_addr(servidor_ip);
	direccion.sin_port = htons(atoi(servidor_puerto));
	
	if (connect(s, (struct sockaddr *)&direccion, 	sizeof (direccion)) == -1)
	{
		fprintf(stderr, "Error. No se puede conectar al servidor\n\r");
		close(s);
		return 1;
	}
	printf("Conexión establecida\n\r");
	
	/**** Paso 3: Enviar mensaje ****/
	
	mensaje[0]='\0';
	strcpy(mensaje,peticion);
	strcat(mensaje,"\n");
	strcat(mensaje,"Host: ");
	strcat(mensaje,argv[1]);
	strcat(mensaje,"\n");
	strcat(mensaje,"Accept: text/html");
	strcat(mensaje,"\n");
	strcat(mensaje, "Accept-Charset:UTF-8");
	strcat(mensaje,"\n");
	strcat(mensaje,"\n");
	printf("%s",mensaje);

	n = strlen(mensaje);
	enviados = write(s, mensaje, n);
	if (enviados == -1 || enviados < n)
	{
		fprintf(stderr, "Error enviando el mensaje\n\r");
		close(s);
		return 1;
	}

	printf("Mensaje enviado\n\r");

	/**** Paso 4: Recibir respuesta ****/

	n = sizeof(respuesta) - 1;
	recibidos = read(s, respuesta, n);
	if (recibidos == 1)
	{
		fprintf(stderr, "Error recibiendo respuesta\n\r");
		close(s);
		return 1;
	}
	respuesta[recibidos] = '\0';
	printf("Respuesta [%d bytes]: %s\n\r", recibidos, respuesta);
	strcpy(fichero_final, "[");
	aux=strtok(respuesta, "[");
	aux=strtok(respuesta, NULL);
	strcat(fichero_final, aux);
	data=json_loads(fichero_final, 0, &error);
	if(!data){
		 fprintf(stderr, "Error: En linea %d: %s\n", error.line, error.text);
		return 1;
	}
	
	if(option==1){
		printf("Listado de edificios de la Universidad de Alicante:\n\r");
		for(i=0; i<json_array_size(data); i++){
			json_t *entry, *nombre;
			const char *message_text;
			
			entry = json_array_get(data, i);
			if(!json_is_object(entry))
			{
				fprintf(stderr, "error: entry %d is not an object\n", i + 1);
				json_decref(data);
				return 1;
			}
			nombre=json_object_get(entry, "nombre");
			if(!json_is_string(nombre)){
				fprintf(stderr, "error: commit %d: sha is not a string\n", i + 1);
				json_decref(data);
				return 1;
			}	
			message_text=json_string_value(nombre);
			printf("%s\n", message_text);
		}
		printf("\n");
		printf("FIN DEL LISTADO DE EDIFICIOS");
	}else if(option==2){
		
	}else if(option==3){
		printf("Listado de edificios en los que existen estancias sin ocupantes asignados:\n\r");
		for(i=0; i<json_array_size(data); i++){
			json_t *entry, *nombre;
			const char *message_text;
			
			entry = json_array_get(data, i);
			
			
			if(!json_is_object(entry)){
				fprintf(stderr, "error: entry %d is not an object\n", i + 1);
				json_decref(root);
				return 1;
			}
			nombre=json_object_get(entry, "nombre");
			
			
			if(!json_is_string(nombre)){
				fprintf(stderr, "error: commit %d: sha is not a string\n", i + 1);
				json_decref(root);
				return 1;
			}	
			
			message_text=json_string_value(nombre);
			printf("%s\n", nombre);
		}
		printf("\n");
		printf("FIN DEL LISTADO DE EDIFICIOS");
	}else if(option==4){
		
	}else{
		
	}

	/**** Paso 5: Cerrar el socket ****/
	close(s);
	printf("Socket cerrado. Comunicación finalizada\n\r");

	return 0;
}