#include <stdio.h>
#include "LinkedList.h"
#include "Dictionary.h"
#include "Parser.h"
#include <string.h>

int main(int argc, char *argv[])
{
    LinkedListf linked_list;
    llf_construct(&linked_list, 10);

    //char str[] = "S{begin}0; A{begin}0; P{run}11; M{allocate}2;\
 O{monitor}7; I{hard drive}8; I{scanner}8; O{projector}20;\
 P{run}6; O{projector}4; M{block}6;\
 I{keyboard}17; M{block}4; O{projector}8; P{run}5; P{run}5;\
 O{hard drive}6; P{run}18; A{finish}0; S{finish}0.";
    char str[] = "lskfjls slkdf jslkf jslfj slfj lsajfwl jfsdljf sljf sljfsdl jfsldfj sdlfj sdlfj sdlfjs lfs fsdlfj slijfsljf lksjf slfj ksfjh skjhf kshfshfksjhfwuhrieh khasfksadjhf ksjhf ksjhfwiheiutweyr 839r 9328 o18u 1[]wF ASODIF OIASF sad fshf[wf ]saf sla s. sss fsa f3u rljsa jjfsa]d fsad fsa fjs fa /fas na.,m nzc,mnv jkchfwaekfj w  waelk jwalk j             lsakdfj lwekjr j ljwejr 0 jlwejr3209 jllskjadf j'as;dflm .,        .m ssdafl kj3lk j lkjfsal dkjalf kjas flsakdj  /. msadfl jl 3j2rl kjlakfjwur903r jaflsad fljsa fsa.m f.s,a s,manf ,samn sa, fs/a .f,samf sa/f safsaf a'sda fa af as lfjlkjsfa 032jr ljf 9387 oiu465 jlwj tlkj lkj l j             23095 ulkaj laskjf slf kj203 9j    ";
    char del = ' ';
    //char* token_pointer = strtok(str,del);
    
   /* while(token_pointer!=NULL)
    {
        printf("%s \n", token_pointer);
        token_pointer = strtok(NULL,del);
    }*/
    int len = 0;
    char** splitted = split_string(str, del ,&len);
    
    /*char* s = "hello   ";
    s = rstrip(s);
    printf("%s", s);
    printf("%s \n" , "world");
    free(s);*/
     
    //return 0; 
    int _i;
    for(_i = 0; _i<len; _i++) 
        printf("%s \n", splitted[_i]);
    for(_i=0; _i<len; _i++)
        free(splitted[_i]);
    free(splitted);    
    return 0; 
    // printf("%s \n", tokenised[1]); 
    llf_append(&linked_list, 1);
    llf_append(&linked_list, 2);
    llf_append(&linked_list, 3);
    llf_append(&linked_list, 4);
    llf_append(&linked_list, 5);
    llf_append(&linked_list, 6);
    llf_append(&linked_list, 7);
    llf_append(&linked_list, 8);
    llf_append(&linked_list, 9);
    llf_append(&linked_list, 10);
   /* llf_remove_at(&linked_list, 0);  
    llf_remove_at(&linked_list, 0);
    
    llf_insert_at(&linked_list, 0, 33);
    llf_append(&linked_list, 91);
    llf_insert_at(&linked_list, 2, 53);*/
    printf("value at 6 : %f \n", llf_get(&linked_list, 6));
    llf_print(&linked_list); 
    }
