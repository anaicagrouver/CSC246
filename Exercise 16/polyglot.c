#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

int main( int argc, char *argv[] )
{
   void *lib_handle;
   char* (*fn)();
   char *error;

   for(int i = 1; i < argc; i++){
         char file[50] = "./";
         strcat(file, argv[i]);
         lib_handle = dlopen(file, RTLD_LAZY);
         if (!lib_handle) 
         {
            fprintf(stderr, "%s\n", dlerror());
            exit(1);
         }

         fn = dlsym(lib_handle, "getMessage");
         if ((error = dlerror()) != NULL)  
         {
            fprintf(stderr, "%s\n", error);
            exit(1);
         }

         (*fn)();
         printf ("%s\n", (*fn)());
         //printf("Valx=%d\n",x);

         dlclose(lib_handle);
   }

  return EXIT_SUCCESS;
}
