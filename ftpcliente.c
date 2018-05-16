// Eduardo Gutierrez Maestro
// Pablo Maroto Lopez


#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <signal.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX 1000000

void INThandler(int);			//funcion que maneja el Ctr+c
void vaciar(char* a);
int Port_Finder(char * buffer)
{
	int port, i;
	char x1[5];
	char x2[5];
	int addit, mult;
	char *tok = strtok(buffer,",");
	char * x3;
	int count = 0;
	while (tok!=NULL)
	{
		if (count == 4)
		{
			snprintf(x1, sizeof(x1), "%s", tok);
		}
		if (count==5)
		{
			snprintf(x2, sizeof(x2), "%s", tok);
		}
		tok = strtok(NULL,",");
		count++;
	}
	for(i=0; i<strlen(x2); i++)
	{
		if(x2[i]==')' || x2[i]=='.')
		{
			x2[i]='\0';
		}
	}

	mult = atoi(x1);
	addit = atoi(x2);
	port = mult*256+addit;

	return port;
}

void CreateFile(const char * buffer, char * name)
{
	FILE * fp;

	fp = fopen(name, "w+"); 

	if(fp==NULL)
	{
		printf("ERROR: fopen\n");
		exit(1);
	}
	fputs(buffer, fp); 

	fclose(fp);
}


int fd, n, fdd, nn;

int main(int argc, char *argv[])
{
	struct timeval tv;
	size_t nbytes;
	int fp;
	fd_set master, slave, master2, slave2;
	char mensaje[MAX];
	char linea[MAX];
	char comando[5], comando2[100];
	struct sockaddr_in cli, serv;
	int puerto, npuerto, fdmax, fdmax2;
	int i, j, result;
	struct hostent *ip;
	char user[MAX], *password;
	int flag=0, flag2=0, contador=3, contador2=0;
	int estado=0;
	char nombre[50];

	tv.tv_sec=10;


	signal(SIGINT, INThandler);				//se asigna la señal SIGINT (Ctr+C) a la funcion INThandler

	if(argc < 2 || argc > 3)
	{
		printf("ERROR: numero de argumentos incorrecto.\n");
		printf("./programa [IP] [PORT]\n");
		exit(-1);
	}
	if(argc==3)
	{
		puerto=atoi(argv[2]);
		ip = gethostbyname(argv[1]);
	}
	else
	{
		puerto=21;
		ip = gethostbyname(argv[1]);
	}

	bzero((char*) &cli, sizeof(cli));
	bzero((char*) &serv, sizeof(serv));

	if((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)				//crear socket
	{
		printf("(Cliente) ERROR al abrir el socket.\n");
		exit(-1);
	}

	serv.sin_family = AF_INET;
	bcopy((char*)ip->h_addr,(char*)&serv.sin_addr.s_addr, ip->h_length);
	serv.sin_port = htons(puerto);

	printf("Usuario: ");
	gets(user);
	//printf("Contraseña: ");
	//gets(password);
	password=getpass("Contraseña: ");

	if(connect(fd, (struct sockaddr *) &serv, sizeof(serv)) == -1)		//conecta el socket cliente con el servidor, bind automatico
	{
		printf("(Cliente) ERROR al asociar IP y puerto al socket.\n");
	}
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	//puts(linea); 	//220 (vsFTPd 3.0.2)

	sprintf(mensaje, "USER %s\n", user);
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	//puts(linea);	//331 Please specify the password.

	sprintf(mensaje, "PASS %s\n", password);
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	puts(linea);	//230 Login successful.
	if(strncmp(linea, "230 ", 4)!=0)
	{
		exit(0);
	}

	sprintf(mensaje, "SYST\n");
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	//puts(linea);	//215 UNIX Type: L8

	sprintf(mensaje, "FEAT\n");
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	//puts(linea);	//211-Features: EPRT EPSV MDTM PASV REST STREAM SIZE TVFS UTF8 211 End

	sprintf(mensaje, "OPTS UTF8 ON\n");
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	//puts(linea);	//200 Always in UTF8 mode.

	sprintf(mensaje, "PWD\n");
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	puts(linea);	//257 "/home/pablo"

	sprintf(mensaje, "TYPE I\n");
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	//puts(linea);	//200 Switching to Binary mode.

	sprintf(mensaje, "PASV\n");
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	//puts(linea);	//227 Entering Passive Mode (127,0,0,1,221,186).

	npuerto=Port_Finder(linea);
	if((fdd = socket(AF_INET, SOCK_STREAM, 0)) == -1)				//crear socket
	{
		printf("(Cliente) ERROR al abrir el socket.\n");
		exit(-1);
	}
	serv.sin_port = htons(npuerto);

	if(connect(fdd, (struct sockaddr *) &serv, sizeof(serv)) == -1)		//conecta el socket cliente con el servidor, bind automatico
	{
		printf("(Cliente) ERROR al asociar IP y puerto al socket.\n");
	}

	sprintf(mensaje, "LIST\n");
	if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
	{
		printf("(Cliente) ERROR write.\n");
	}

	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	puts(linea);
	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fdd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	puts(linea);
	memset(&linea[0], 0, sizeof(linea));
	if((n = read(fd, linea, MAX)) == -1)
	{
		printf("(Cliente) ERROR read.\n");
	}
	puts(linea);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	FD_ZERO(&master);
	FD_SET(fd, &master);
	FD_SET(0, &master);

	do
	{
		if(fd>fdd)
		{
			fdmax=fd;
		}
		else
			fdmax=fdd;

		memset(&mensaje[0], 0, sizeof(mensaje));
		memset(&linea[0], 0, sizeof(linea));
		memset(&comando[0], 0, sizeof(comando));
		
		slave=master;

		if((result=select(fdmax+1, &slave, NULL, NULL, &tv))==-1)
		{
			printf("ERROR: select.\n");
		}

		for(i=0; i<=fdmax; i++)
		{
			if(FD_ISSET(i, &slave))
			{
				if(i==0)
				{
					gets(mensaje);

					flag2=0;
					j=0;
					memset(&comando2[0], 0, sizeof(comando2));
					for(i=0; i<strlen(mensaje); i++)
					{
						if(mensaje[i]==' ')
						{
							comando[i]='\0';
							flag2=1;
						}
						if(mensaje[i]!=' ' && flag2==0)
						{
							comando[i]=mensaje[i];
						}

						if(mensaje[i]!=' ' && flag2==1)
						{
							comando2[j]=mensaje[i];
							j++;
						}
					}

					if(strcmp(mensaje, "bye")==0)
					
						break;
					

					else if(strcmp(mensaje, "pwd")==0)
					{
						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=1;
					}

					else if(strcmp(comando, "cd")==0)
					{
						strcpy(mensaje, "CWD ");
						strcat(mensaje, comando2);
						
						if(strcmp(comando2, "..")==0)
						{
							strcpy(mensaje, "CDUP");
						}

						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=2;
					}

					else if(strcmp(comando, "mkdir")==0)
					{
						strcpy(mensaje, "MKD ");
						strcat(mensaje, comando2);
						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=2;

					}

					else if(strcmp(comando, "rmdir")==0)
					{
						strcpy(mensaje, "RMD ");
						strcat(mensaje, comando2);
						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=2;

					}

					else if(strcmp(comando, "rm")==0)
					{
						strcpy(mensaje, "DELE ");
						strcat(mensaje, comando2);
						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=2;
					}

					else if(strcmp(mensaje, "ls")==0)
					{
						sprintf(mensaje, "PASV\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=3;
					}

					else if(strcmp(comando, "get")==0)
					{
						sprintf(mensaje, "PASV\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=4;
					}
					else if(strcmp(comando, "put")==0)
					{
						sprintf(mensaje, "PASV\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=5;
					}
					else if(strcmp(mensaje, "clear")==0)
					{
						system("clear");
					}
					else if(strcmp(comando, "rename")==0)
					{
						strcpy(mensaje, "RNFR ");
						strcat(mensaje, comando2);

						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=6;
					}
					else
					{
						printf("\t\tUSO DE COMANDOS\n\n");
						printf("·ls\t\t\t\tListado de directorios.\n");
						printf("·cd <nuevo directorio>\t\tCambio de directorio.\n");
						printf("·pwd\t\t\t\tMuestra directorio actual.\n");
						printf("·clear\t\t\t\tLimpia la pantalla.\n");
						printf("·get <nombre del archivo>\tBajar un archivo.\n");
						printf("·put <nombre del archivo>\tBajar un archivo.\n");
						printf("·mkdir <nombre directorio>\tCrear un directorio.\n");
						printf("·rmdir <nombre directorio>\tBorrar un directorio.\n");
						printf("·rename <nombre del archivo>\tRenombrar un archivo\n");
						printf("·help\t\t\t\tMuestra este menu.\n");
						printf("·bye\t\t\t\tSalir de la aplicacion.\n\n");
					}
				}
				if(i==fd)
				{
					if(estado==1 || estado==2 || estado==10 || estado==11 || estado==20)
					{
						memset(&linea[0], 0, sizeof(linea));
						if((n = read(fd, linea, MAX)) == -1)
						{
							printf("(Cliente) ERROR read.\n");
						}
						puts(linea);
					}
					if(estado==12)
					{
							
						if(flag==0)
						{
							memset(&linea[0], 0, sizeof(linea));
							if((n = read(fd, linea, MAX)) == -1)
							{
								printf("(Cliente) ERROR read.\n");
							}
							puts(linea);
							fp = open(comando2, O_RDONLY);
							flag=1;
						}

						if(fp==-1)
						{
							printf("ERROR: open\n");
							exit(1);
						}
						

						
						memset(&linea[0], 0, sizeof(linea));
						while((n=read(fp, &linea, sizeof(linea)))!=0)
						{
							write(fdd, linea, n);
						}
						
							
						FD_CLR(fdd, &master);
						close(fp);
						close(fdd);
						flag=0;
						estado=1;
					}

					if(estado==3)
					{
						memset(&linea[0], 0, sizeof(linea));
						if((n = read(fd, linea, MAX)) == -1)
						{
							printf("(Cliente) ERROR read.\n");
						}
						npuerto=Port_Finder(linea);
						if((fdd = socket(AF_INET, SOCK_STREAM, 0)) == -1)				//crear socket
						{
							printf("(Cliente) ERROR al abrir el socket.\n");
							exit(-1);
						}
						serv.sin_port = htons(npuerto);

						if(connect(fdd, (struct sockaddr *) &serv, sizeof(serv)) == -1)		//conecta el socket cliente con el servidor, bind automatico
						{
							printf("(Cliente) ERROR al asociar IP y puerto al socket.\n");
						}

						FD_SET(fdd, &master);

						strcpy(mensaje, "LIST");
						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=10;
					}
					if(estado==4)
					{
						memset(&linea[0], 0, sizeof(linea));
						if((n = read(fd, linea, MAX)) == -1)
						{
							printf("(Cliente) ERROR read.\n");
						}
						npuerto=Port_Finder(linea);
						if((fdd = socket(AF_INET, SOCK_STREAM, 0)) == -1)				//crear socket
						{
							printf("(Cliente) ERROR al abrir el socket.\n");
							exit(-1);
						}
						serv.sin_port = htons(npuerto);

						if(connect(fdd, (struct sockaddr *) &serv, sizeof(serv)) == -1)		//conecta el socket cliente con el servidor, bind automatico
						{
							printf("(Cliente) ERROR al asociar IP y puerto al socket.\n");
						}

						FD_SET(fdd, &master);

						strcpy(mensaje, "RETR ");
						strcat(mensaje, comando2);
						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=11;
					}
					if(estado==5)
					{
						memset(&linea[0], 0, sizeof(linea));
						if((n = read(fd, linea, MAX)) == -1)
						{
							printf("(Cliente) ERROR read.\n");
						}
						npuerto=Port_Finder(linea);
						if((fdd = socket(AF_INET, SOCK_STREAM, 0)) == -1)				//crear socket
						{
							printf("(Cliente) ERROR al abrir el socket.\n");
							exit(-1);
						}
						serv.sin_port = htons(npuerto);

						if(connect(fdd, (struct sockaddr *) &serv, sizeof(serv)) == -1)		//conecta el socket cliente con el servidor, bind automatico
						{
							printf("(Cliente) ERROR al asociar IP y puerto al socket.\n");
						}

						FD_SET(fdd, &master);

						strcpy(mensaje,  "STOR ");
						strcat(mensaje, comando2);
						strcat(mensaje, "\r\n");
						if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
						{
							printf("(Cliente) ERROR write.\n");
						}
						estado=12;
					}
					if(estado==6)
					{
						memset(&linea[0], 0, sizeof(linea));
						if((n = read(fd, linea, MAX)) == -1)
						{
							printf("(Cliente) ERROR read.\n");
						}
						if(strncmp(linea, "350", 3)==0)
						{
							printf("Nombre: ");
							gets(nombre);
							strcpy(mensaje, "RNTO ");
							strcat(mensaje, nombre);
							strcat(mensaje, "\r\n");
							if(write(fd, mensaje, strlen(mensaje)) != strlen(mensaje))	//envio de mensaje
							{
								printf("(Cliente) ERROR write.\n");
							}
							estado=20;
						}
						else
						{
							puts(linea);
						}
					}
				}
				if(i==fdd)
				{
					if(estado==10)
					{
						memset(&linea[0], 0, sizeof(linea));
						if((n = read(fdd, linea, MAX)) == -1)
						{
							printf("(Cliente) ERROR read.\n");
						}
						if(n!=0)
						{
							puts(linea);
						}
						else
						{
							FD_CLR(fdd, &master);
						}
					}

					if(estado==11)
					{
						if(flag==0)
						{
							fp = open(comando2, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH);
							flag=1;
						}

						if(fp==-1)
						{
							printf("ERROR: open\n");
							exit(1);
						}
						

						
						memset(&linea[0], 0, sizeof(linea));
						if((n = read(fdd, linea, MAX)) == -1)
						{
							printf("(Cliente) ERROR read.\n");
						}
						if(n>0)
						{
							nbytes=sizeof(linea);
							write(fp, linea, n);
						}
						else
						{
							FD_CLR(fdd, &master);
							close(fp);
							flag=0;
						}
					}
				}
			}
		}
	}
	while(strcmp(mensaje, "bye")!=0);
	printf("byeeeeeeeeeeee!\n");

	close(fd);
	close(fdd);
	exit(0);
}

void  INThandler(int sig)
{
	close(fd);
	close(fdd);
  signal(sig, SIG_IGN);

    printf("...cerrando conexion\n");
    exit(0);
}