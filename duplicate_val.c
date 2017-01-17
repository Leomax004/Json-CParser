#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include "nxjson.h"

typedef struct list
{
   const char *data;
    struct list *next;
    
}node;
node *hash_table[50000];

#define ERROR(msg, p) fprintf(stderr, "ERROR: " msg " %s\n", (p));
static char* load_file(const char* filepath) {
    struct stat st;
    if (stat(filepath, &st)==-1) {
        // ERROR("can't find file", filepath);
        return 0;
    }
    int fd=open(filepath, O_RDONLY);
    if (fd==-1) {
        ERROR("can't open file", filepath);
        return 0;
    }
    char* text=malloc(st.st_size+1); // this is not going to be freed
    if (st.st_size!=read(fd, text, st.st_size)) {
        ERROR("can't read file", filepath);
        close(fd);
        return 0;
    }
    close(fd);
    text[st.st_size]='\0';
    
    
    return text;
}


unsigned long hash_func(const char *str)
{
    unsigned long hash = 5381;
    int c;
    
    while (*str++)
    {   c=*str;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
        str++;
    }
    // printf("%ld\n",hash%80000);
    
    return hash%50000;
}

/*when the hash value is new */
node *create(node *liststart,const char *strings)
{
    
  node *temp;
    
    temp=(struct list *)malloc(sizeof(node));
    if(!temp)
    {
        printf("Memory allocation failed\n");
    }
    
    temp->data=strings;
    temp->next=NULL;
    
    return temp;
}

void process_nx_json_array(nx_json *js, int count)
{
    printf("JSON_ARRAY\n");
    
    node *temp,*ptr1,*start;
    
    start=NULL;
    
    memset(hash_table,0,sizeof(hash_table));
    
    
    for(int j = 0;j < js->length; j++)
    {
        const nx_json* item1=nx_json_item(js,j);
        
        const   char * const p1=nx_json_get(item1,"PartNumber")->text_value;
        
        temp = create(NULL,p1);
        
        int  m = hash_func(p1);
        
        node **ptr2;
        
        ptr2 = &hash_table[m];
        
        if(*ptr2 == NULL)
        {
            *ptr2 = temp;
        }
        else
        {
            ptr1 = *ptr2;
            
            while(ptr1->next != NULL)
            {
                ptr1 = ptr1->next;
                
            }
            ptr1->next=temp;
        }
        
        
    }

    
    printf("Duplicates Part Number are as follows\n");
    node *prev,*pnext;
    
    int ret,p = 0;
    
    for(int j = 0;j < 50000;j++)
        
    {
       
            prev = hash_table[j];
            
            while(prev != NULL)
            {
                pnext = prev->next;
                
                while(pnext != NULL)
                {
                    
                    ret= strcmp(prev->data,pnext->data);
                    
                    if(ret == 0)
                    {
                        p++;
                        
                        printf("%s\n",prev->data);
                    }
                    
                    pnext = pnext->next;
                }
                
                
                prev = prev->next;
            }
            
        
        
    }
    
    printf("total number of duplicate value%d\n",p);
    
    
    
    
    
}


int main()
{
    
        char *input = load_file("./ProductList_Oct10_2015.json");
    
        char *expected_output = load_file("./ProductList_Oct10_2015.json");
        
        const nx_json* json = nx_json_parse_utf8(input);
        
        nx_json *js = json->child ;
        
        
        
        int count = 0;
        
        for(js = json->child ; js != NULL; js = js->next)
        {
            
            switch(js->type)
            {
                case NX_JSON_NULL:
                    
                    printf("JSON_NULL\n");
                    break;
                    
                case NX_JSON_OBJECT:
                    
                    printf("JSON_OBJECT\n");
                    
                    break;
                    
                case NX_JSON_ARRAY:
                    
                    process_nx_json_array(js,count);
                    
                    break ;
                    
                case NX_JSON_STRING:
                    
                    printf("JSON_STRING\n");
                    break;
                    
                case NX_JSON_INTEGER:
                    
                    printf("JSON_INTEGER\n");
                    
                    break;
                    
                case NX_JSON_DOUBLE:
                    printf("JSON_DOUBLE\n");
                    
                    break;
                    
                case NX_JSON_BOOL:
                    
                    printf("JSON_BOOL\n");
                    break;
                    
                default:
                    printf("JSON_DEFAULT\n");
                    
                    break; 
                    
            } //switch
            
        }//for nxjson  
        
        // printf("%d",count);
        if(input) free(input);
        
        if(expected_output) free(expected_output);
    
    return 0;
}
