/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "motd.h"


void
motd_prog_1(char *host, char* message)

{
	CLIENT *clnt;
	//int  *result_1;
	char *count_1_arg;
	//char * *result_2;
	//int  get_1_arg;
	void  *result_3;
	char * add_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, MOTD_PROG, MOTD_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	add_1_arg = message;
	// result_1 = count_1((void*)&count_1_arg, clnt);
	// if (result_1 == (int *) NULL) {
	// 	clnt_perror (clnt, "call failed");
	// }
	// result_2 = get_1(&get_1_arg, clnt);
	// if (result_2 == (char **) NULL) {
	// 	clnt_perror (clnt, "call failed");
	// }


	//char *count_1_arg;
	int *np = count_1((void*)&count_1_arg, clnt);
	if ( np == (int *) NULL ) {
		clnt_perror (clnt, "call failed");
		exit( EXIT_FAILURE );
	}
	if(add_1_arg == NULL){
		for ( int i = 0; i < *np; i++ ) {
			char **spp = get_1(&i, clnt);
			if ( spp == (char **) NULL ) {
				clnt_perror (clnt, "call failed");
				exit( EXIT_FAILURE );
			}
			printf( "%s\n", *spp );
		}
	}
	if(add_1_arg != NULL){
		result_3 = add_1(&add_1_arg, clnt);
		if (result_3 == (void *) NULL) {
			clnt_perror (clnt, "call failed");
		}
	}


#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	if (argc < 2 || argc > 3 ) {
		printf ("usage: %s server_host [message]\n", argv[0]);
		exit (1);
	}
	if ( argc == 3 )
		motd_prog_1( argv[ 1 ], argv[ 2 ] );
	else
		motd_prog_1( argv[ 1 ], NULL );
	exit( EXIT_SUCCESS );
}