#include "free_memory.h"

void free_instruction_split(char** intructrReadSplitBySpaces){
	int array_size = string_array_size(intructrReadSplitBySpaces);
	for(int i = 0; i < array_size; i++){
		free(intructrReadSplitBySpaces[i]);
	}
}
