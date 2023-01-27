/**
 * 
 * @brief header usato per gestire i file
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <queue.h>


void is_correct_file(char *fullname, struct stat filestat, node **support_queue);

void recursiveSearch(char *directory, node **support_queue);

