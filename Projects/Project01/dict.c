//A dictionary
//data is the contents of the dictionary (not including definitions)
//so dict.data[0] should be a string containting the first word in the dictionary, and so on.
//size is the number of words in the dictionary. 
typedef struct{
  char** data;
  int size;
} dictionary;


//reads a dictionary line by line from filename into the data structure.
//aborts if the file is not opened.

void dict_open(char* filename, dictionary* dict) {

  //open the file, make sure it works
  FILE* f=fopen(filename,"r");
  if (f==NULL) {
    fprintf(stderr,"File %s not opened properly\n",filename);
    exit(1);
  }

  //Start small-ish, grow as needed.
  long int dict_capacity=1000;

  dict->data=malloc(dict_capacity*sizeof(char*));
  
  //how many are filled?
  int dict_filled=0;

  //a buffer to read into
  char word[1000];
  int wordlen;
  
  //Start reading from file.  Keep reading as long as results make sense. 
  while (fgets(word,1000,f)!=NULL) {
    
    wordlen=strlen(word);
    //strip the newline if it is there:
    if (word[wordlen-1]=='\n') {
      word[wordlen-1]='\0';
    }
    

    //if the dictionary is currently full, expand it.
    if (dict_filled==dict_capacity-1) {
      dict_capacity=dict_capacity*2;
      dict->data=realloc(dict->data,dict_capacity*sizeof(char*));
    }
    //malloc room for the word -- I bet this is real slow, but good on memory.
    dict->data[dict_filled]=malloc((wordlen+1)*sizeof(char));
    //copy the word into dictionary.
    strcpy(dict->data[dict_filled],word);
    //increment len
    dict_filled++;
  }

  //Finish
  dict->size=dict_filled;
  fclose(f);
  
}


//checks to see if word is in the supplied dictionary.
//if it is, the return value is a non-negative integer indicating the position of the word in the dictionary.
//if it is not, returns -1.
//
//*ASSUMES THAT dict.data IS SORTED IN ALPHABETICAL ORDER!
int dict_lookup(char* word, dictionary* dict) {

  //will hold the index of word in dict, if word exists in dict. 
  int targ_idx=-1;

  //upper and lower bounds for binary search. 
  int l=0;
  int u=dict->size-1;

  int m;
  
  if (strcmp(word, dict->data[l])==0) {
    targ_idx=l;
  }

  if (strcmp(word,dict->data[u])==0) {
    targ_idx=u;
  }

  int r;
  while (targ_idx==-1 && u-l>1) {
    m=(l+u)/2;
    //get the result of alphabetic comparison of the target word and the m'th word in the dictionary. 
    r=strcmp(word,dict->data[m]);
    //found!
    if (r==0) {
      targ_idx=m;
    }
    //update l and u accordingly. 
    else if (r<0){
      u=m;
    } else {
      l=m;
    }
  }
  
  return targ_idx;
}
