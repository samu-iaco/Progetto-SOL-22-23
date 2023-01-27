#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <file.h>



void is_correct_file(char *fullname, struct stat filestat, node **support_queue)
{
    if ((stat(fullname, &filestat)) == -1)
    {
        fprintf(stderr, "il file %s non esiste\n", fullname);
    }

    else if (S_ISREG(filestat.st_mode))
    {
        //ottengo l'estensione del file
        size_t lenext = strlen(strrchr(fullname,'.')) + 1;
        char *ext = malloc(sizeof(char) * lenext);
        if(ext == NULL)
            exit(EXIT_FAILURE);
        memset(ext,0,lenext);
        memcpy(ext,strrchr(fullname,'.'), lenext);

        // printf("estensione del file: %s -----> %s\n", fullname, ext);

        //se il file è corretto lo aggiungo alla lista
        if (filestat.st_size % 8 == 0 && (strcmp(ext,".dat") == 0))
        {
            addTail(support_queue, fullname, -1);
            free(ext);
        }
    }

}

void recursiveSearch(char *directory, node **support_queue)
{
    DIR *dir;
    struct dirent *entry;
    struct stat filestat;

    dir = opendir(directory);
    if(dir == NULL){
        perror("opendir");
        return ;
    }

    while ((entry = readdir(dir)))
	{
		char fullname[FILENAME_MAX];
		sprintf(fullname, "%s/%s", directory, entry->d_name);

		stat(fullname, &filestat);

		
        //è una cartella
		if (S_ISDIR(filestat.st_mode))
		{
			//printf("%4s: %s\n", "Dir", fullname);
			
			if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
			{
				recursiveSearch(fullname,support_queue);
			}
		}
        //è un file
		else if(S_ISREG(filestat.st_mode))
		{
            is_correct_file(fullname,filestat,support_queue);   //controllo se il file è corretto
		}
	}

    if(closedir(dir) == -1){
        perror("closedir");
        exit(EXIT_FAILURE);
    }
}