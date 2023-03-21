
#include "memory.h"

bool mem_cmp(const char *a, const char *b, int size){
    for(int i = 0; i < size; i++){
        if(a[i] != b[i]){
            return false;
        }
    }
    return true;
}

bool str_cmp(const char *a, const char *b){
    while(true){
        if(*a != *b){
            return false;
        }
        if(*a =='\0'){
            break;
        }
        if(*b =='\0'){
            break;
        }
        a++;
        b++;
    }
    return true;
}

int mem_find(const char *mem, char search){
    int index = 0;
    while(*mem != search){
        index++;
        mem++;
    }
    return index;
}

int mem_find(const char *mem, int size, char search){
    int index = 0;
    while(*mem != search){
        if(index >= size){
            return -1;
        }
        index++;
        mem++;
    }
    if(index >= size){
        return -1;
    }
    return index;
}

int str_len(const char *str){
    return mem_find(str, '\0');
}

char to_upper(char c){
    if(c >= 'a' && c <= 'z'){
        return c - 'a' + 'A';
    }else{
        return c;
    }
}

char to_lower(char c){
    if(c >= 'A' && c <= 'Z'){
        return c - 'A' + 'a';
    }else{
        return c;
    }
}
