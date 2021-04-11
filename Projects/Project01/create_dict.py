#!/usr/bin/env python
import sys

argc=len(sys.argv)

if argc<3:
    print "Usage: create_dict.py raw_text_file output_dictionary_file <min_word_len> <max_word_len> <include words with apostrophe's?> <<max dictionary length>"
    print "options:"
    print "min_word_len -- words shorter than this are not included. Default to 1"
    print "max_word_len -- words longer than this are not included. Default to 26"
    print "include words with Apostrphe -- T or F.  If F, then words that have apostrophes in the are not included. Defaults to T"
    print "max dictionary length -- don't return more than this many words in the dictionary.  Defaults to 1,000,000"
    exit()
    
#Raw text file
text_file=sys.argv[1];
#output file
dict_file=sys.argv[2];
#only include words of this size or longer
if argc >= 4:
    min_word_len=int(sys.argv[3]);
else:
    min_word_len=1
    
#only include words of this size or shorter
if argc >=5:
    max_word_len=int(sys.argv[4]);
else:
    max_word_len=26;

#include words with apostrophes?
if argc >= 6:
    if sys.argv[5]=='T':
        include_apost=True;
    else:
        include_apost=False;
else:
    include_apost=True;

#Only include the first this many words
if argc >= 7:
    max_dict_len=int(sys.argv[6])
else:
    max_dict_len=10000000

print "Raw File: "+text_file+"\nDict File: "+dict_file+"\nmin_word_len: "+str(min_word_len)+"\nmax_word_len: "+str(max_word_len)+"\ninclude words with apostrophes?: "+str(include_apost)+"\nmax dictionary length: "+str(max_dict_len)
      
#Get the words
wordlist=open(text_file,"r").readlines();

#strip whitespace, lowercase it
wordlist=[w.strip().lower() for w in wordlist]

#start filtering
wordlist=[w for w in wordlist if len(w)>=min_word_len];
wordlist=[w for w in wordlist if len(w)<=max_word_len];

if include_apost==False:
    wordlist=[w for w in wordlist if '\'' not in w]

wordlist=sorted(wordlist);

max_dict_len=min(max_dict_len,len(wordlist));

wordlist=wordlist[0:max_dict_len];

wordlist=[w+('\n') for w in wordlist]

f=open(dict_file,"w")
f.writelines(wordlist);
f.close();

    
    
    
