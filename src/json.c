#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

#include <assert.h>

#include "json.h"

static const char *ep;

const char * json_get_error_ptr(void) {
    return ep;
}

static int json_strcasecmp(const char *s1,const char *s2){
    if (!s1){
        return (s1==s2)?0:1;
    }

    if (!s2){
        return 1;
    }

    for(; tolower(*s1) == tolower(*s2); ++s1, ++s2){
        if(*s1 == 0){
            return 0;
        }
    }

    return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

static void *(*json_malloc)(size_t sz) = malloc;
static void (*json_free)(void *ptr) = free;

static char* json_strdup(const char* str){
      size_t len;
      char* copy;

      len = strlen(str) + 1;
      if (!(copy = (char*)json_malloc(len))) return 0;
      memcpy(copy,str,len);
      return copy;
}

void json_init_hooks(json_hooks * hooks)
{
    if (!hooks) {
        json_malloc = malloc;
        json_free = free;
        return;
    }

    json_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
    json_free   = (hooks->free_fn)?hooks->free_fn:free;
}

static Json *json_new_item(void){
    Json * node = (Json*)json_malloc(sizeof(Json));

    if (node){
        memset(node,0,sizeof(Json));
    }
    return node;
}

void json_delete(Json *c){
    Json *next;

    while (c){
        next=c->next;
        if(!(c->type&json_is_reference) && c->child)
            json_delete(c->child);

        if(!(c->type&json_is_reference) && c->valuestring)
            json_free(c->valuestring);

        if(c->string) json_free(c->string);
            json_free(c);

        c=next;
    }
}

static const char *parse_number(Json *item,const char *num){
    double n=0,sign=1,scale=0;
    int subscale=0,signsubscale=1;

    if (*num=='-')
        sign=-1,num++;

    if (*num=='0')
        num++;

    if (*num>='1' && *num<='9')
    do    n=(n*10.0)+(*num++ -'0');

    while (*num>='0' && *num<='9');
    if (*num=='.' && num[1]>='0' && num[1]<='9') {
        num++;
        do n=(n*10.0)+(*num++ -'0'),scale--;
            while (*num>='0' && *num<='9');
    }

    if (*num=='e' || *num=='E'){
        num++;
        if (*num=='+')
            num++;
        else if (*num=='-')
            signsubscale=-1,num++;

        while (*num>='0' && *num<='9')
            subscale=(subscale*10)+(*num++ - '0');
    }

    n=sign*n*pow(10.0,(scale+subscale*signsubscale));

    item->valuedouble=n;
    item->valueint=(int)n;
    item->type=json_number;
    return num;
}

static char *print_number(Json *item){
    char *str;
    double d=item->valuedouble;

    if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN){
        str=(char*)json_malloc(21);
        if (str)
           sprintf(str,"%d",item->valueint);
    }else{
        str=(char*)json_malloc(64);
        if (str){
            if (fabs(floor(d)-d)<=DBL_EPSILON && fabs(d)<1.0e60)
                sprintf(str,"%.0f",d);
            else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)
                sprintf(str,"%e",d);
            else
                sprintf(str,"%f",d);
        }
    }

    return str;
}

static unsigned parse_hex4(const char *str){
    unsigned h=0;

    if (*str>='0' && *str<='9')
        h+=(*str)-'0';
    else if (*str>='A' && *str<='F')
        h+=10+(*str)-'A';
    else if (*str>='a' && *str<='f')
        h+=10+(*str)-'a';
    else return 0;

    h=h<<4;str++;
    if (*str>='0' && *str<='9')
        h+=(*str)-'0';
    else if (*str>='A' && *str<='F')
        h+=10+(*str)-'A';
    else if (*str>='a' && *str<='f')
        h+=10+(*str)-'a';
    else return 0;

    h=h<<4;str++;
    if (*str>='0' && *str<='9')
        h+=(*str)-'0';
    else if (*str>='A' && *str<='F')
        h+=10+(*str)-'A';
    else if (*str>='a' && *str<='f')
        h+=10+(*str)-'a';
    else return 0;

    h=h<<4;str++;
    if (*str>='0' && *str<='9')
        h+=(*str)-'0';
    else if (*str>='A' && *str<='F')
        h+=10+(*str)-'A';
    else if (*str>='a' && *str<='f')
        h+=10+(*str)-'a';
    else return 0;

    return h;
}

static const unsigned char first_byte_mark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char *parse_string(Json *item,const char *str){
   const char *ptr=str+1;char *ptr2;char *out;int len=0;unsigned uc,uc2;
   if (*str!='\"'){
       ep=str;
       return 0;
   }

    while (*ptr!='\"' && *ptr && ++len)
        if (*ptr++ == '\\')
            ptr++;

    out=(char*)json_malloc(len+1);
    if (!out) return 0;

    ptr=str+1;ptr2=out;
    while (*ptr!='\"' && *ptr){
        if (*ptr!='\\')
            *ptr2++=*ptr++;
        else{
            ptr++;
            switch (*ptr){
                case 'b':
                    *ptr2++='\b';
                break;
                case 'f':
                    *ptr2++='\f';
                break;
                case 'n':
                    *ptr2++='\n';
                break;
                case 'r':
                    *ptr2++='\r';
                break;
                case 't':
                     *ptr2++='\t';
                break;
                case 'u':
                    uc=parse_hex4(ptr+1);
                    ptr+=4;
                    if ((uc>=0xDC00 && uc<=0xDFFF) || uc==0)
                        break;

                    if (uc>=0xD800 && uc<=0xDBFF){
                        if (ptr[1]!='\\' || ptr[2]!='u')
                            break;

                        uc2=parse_hex4(ptr+3);ptr+=6;

                        if (uc2<0xDC00 || uc2>0xDFFF)
                            break;

                        uc=0x10000 + (((uc&0x3FF)<<10) | (uc2&0x3FF));
                    }

                    len=4;if (uc<0x80) len=1;else if (uc<0x800) len=2;else if (uc<0x10000) len=3; ptr2+=len;

                    switch (len) {
                        case 4: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                        case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                        case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
                        case 1: *--ptr2 =(uc | first_byte_mark[len]);
                    }

                    ptr2+=len;
            break;
                default:
                    *ptr2++=*ptr;
                break;
            }
            ptr++;
        }
    }

    *ptr2=0;
    if (*ptr=='\"')
        ptr++;
    item->valuestring=out;
    item->type=json_string;
    return ptr;
}

static char *print_string_ptr(const char *str){
    const char *ptr;
    char *ptr2,*out;
    int len=0;
    unsigned char token;

    if (!str)
        return json_strdup("");

    ptr=str;

    while ((token=*ptr) && ++len) {
        if (strchr("\"\\\b\f\n\r\t",token))
            len++;
        else if(token<32)
            len+=5;
        ptr++;
    }

    out=(char*)json_malloc(len+3);
    if (!out)
       return 0;

    ptr2=out;
    ptr=str;
    *ptr2++='\"';
    while (*ptr){
        if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\')
            *ptr2++=*ptr++;
        else{
            *ptr2++='\\';
            switch (token=*ptr++){
                case '\\':
                    *ptr2++='\\';
                break;
                case '\"':
                    *ptr2++='\"';
                break;
                case '\b':
                    *ptr2++='b';
                break;
                case '\f':
                    *ptr2++='f';
                break;
                case '\n':
                    *ptr2++='n';
                break;
                case '\r':
                    *ptr2++='r';
                break;
                case '\t':
                    *ptr2++='t';
                break;
                default:
                    sprintf(ptr2,"u%04x",token);ptr2+=5;
                break;
            }
        }
    }

    *ptr2++='\"';*ptr2++=0;
    return out;
}

static char *print_string(Json *item){
    return print_string_ptr(item->valuestring);
}

static const char *parse_value(Json *item,const char *value);
static char *print_value(Json *item,int depth,int fmt);
static const char *parse_array(Json *item,const char *value);
static char *print_array(Json *item,int depth,int fmt);
static const char *parse_object(Json *item,const char *value);
static char *print_object(Json *item,int depth,int fmt);

static const char *skip(const char *in) {
    while (in && *in && (unsigned char)*in<=32)
        in++;
    return in;
}

Json *json_parse_with_opts(const char *value,const char **return_parse_end,int require_null_terminated){
    const char *end=0;
    Json *c=json_new_item();
    ep=0;
    if (!c)
        return 0;

    end=parse_value(c,skip(value));
    if (!end){
        json_delete(c);
        return 0;
    }

    if (require_null_terminated) {
        end=skip(end);
        if (*end) {
            json_delete(c);
            ep=end;
            return 0;
        }
    }

    if (return_parse_end)
        *return_parse_end=end;

    return c;
}

Json * json_parse(const char *value) {
    return json_parse_with_opts(value,0,0);
}

char * json_print(Json *item){
    assert(item);

    return print_value(item,0,1);
}

char * json_print_unformatted(Json *item)    {return print_value(item,0,0);}

static const char *parse_value(Json *item,const char *value){
    if (!value)
        return 0;

    if (!strncmp(value,"null",4)){
        item->type=json_null;
        return value+4;
    }

    if (!strncmp(value,"false",5)){
        item->type=json_false;
        return value+5;
    }

    if (!strncmp(value,"true",4)){
        item->type=json_true;
        item->valueint=1;
        return value+4;
    }

    if (*value=='\"'){
        return parse_string(item,value);
    }

    if (*value=='-' || (*value>='0' && *value<='9')){
        return parse_number(item,value);
    }

    if (*value=='['){
        return parse_array(item,value);
    }

    if (*value=='{'){
        return parse_object(item,value);
    }

    ep=value;return 0;
}

static char *print_value(Json *item,int depth,int fmt){
    char *out=0;
    if (!item) return 0;
    switch ((item->type)&255){
        case json_null:
            out=json_strdup("null");
        break;
        case json_false:
            out=json_strdup("false");
        break;
        case json_true:
            out=json_strdup("true");
        break;
        case json_number:
            out=print_number(item);
        break;
        case json_string:
            out=print_string(item);
        break;
        case json_array:
            out=print_array(item,depth,fmt);
        break;
        case json_object:
            out=print_object(item,depth,fmt);
        break;
    }
    return out;
}

static const char *parse_array(Json *item,const char *value){
    Json *child;
    if (*value!='[')    {ep=value;return 0;}

    item->type=json_array;
    value=skip(value+1);
    if (*value==']') return value+1;

    item->child=child=json_new_item();
    if (!item->child) return 0;
        value=skip(parse_value(child,skip(value)));

    if (!value)
        return 0;

    while (*value==','){
        Json *new_item;
        if (!(new_item=json_new_item()))
            return 0;
        child->next=new_item;new_item->prev=child;child=new_item;
        value=skip(parse_value(child,skip(value+1)));
        if (!value)
            return 0;
    }

    if (*value==']')
        return value+1;

    ep=value;
    return 0;
}

static char *print_array(Json *item,int depth,int fmt){
    char **entries;
    char *out=0,*ptr,*ret;int len=5;
    Json *child=item->child;
    int numentries=0,i=0,fail=0;

    while (child){
        numentries++,child=child->next;
    }

    if (!numentries){
        out=(char*)json_malloc(3);
            if (out) strcpy(out,"[]");
                return out;
    }

    entries=(char**)json_malloc(numentries*sizeof(char*));
    if (!entries){
        return 0;
    }

    memset(entries,0,numentries*sizeof(char*));
    child=item->child;
    while (child && !fail){
        ret=print_value(child,depth+1,fmt);
        entries[i++]=ret;
        if (ret)
            len+=strlen(ret)+2+(fmt?1:0);
        else
            fail=1;
        child=child->next;
    }

    if (!fail)out=(char*)json_malloc(len);
    if (!out)fail=1;

    if (fail){
        for (i=0;i<numentries;i++){
            if (entries[i])json_free(entries[i]);
        }
        json_free(entries);
        return 0;
    }

    *out='[';
    ptr=out+1;*ptr=0;
    for (i=0;i<numentries;i++){
        strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
        if (i!=numentries-1) {
                *ptr++=',';
                if(fmt)*ptr++=' ';
                *ptr=0;
         }
         json_free(entries[i]);
    }
    json_free(entries);
    *ptr++=']';
    *ptr++=0;
    return out;
}

static const char *parse_object(Json *item,const char *value){
    Json *child;
    if (*value!='{'){
        ep=value;
        return 0;
    }

    item->type=json_object;
    value=skip(value+1);
    if (*value=='}'){
        return value+1;
    }

    item->child=child=json_new_item();
    if (!item->child)
        return 0;

    value=skip(parse_string(child,skip(value)));
    if (!value)
        return 0;

    child->string=child->valuestring;child->valuestring=0;
    if (*value!=':') {
        ep=value;
        return 0;
    }
    value=skip(parse_value(child,skip(value+1)));
    if (!value) return 0;

    while (*value==','){
        Json *new_item;
        if (!(new_item=json_new_item()))
            return 0;

        child->next=new_item;new_item->prev=child;child=new_item;
        value=skip(parse_string(child,skip(value+1)));
        if (!value){
            return 0;
        }
        child->string=child->valuestring;child->valuestring=0;
        if (*value!=':') {
            ep=value;return 0;
        }
    value=skip(parse_value(child,skip(value+1)));
    if (!value)
            return 0;
    }

    if (*value=='}')
        return value+1;
    ep=value;
    return 0;
}

static char *print_object(Json *item,int depth,int fmt){
    char **entries=0,**names=0;
    char *out=0,*ptr,*ret,*str;int len=7,i=0,j;
    Json *child=item->child;
    int numentries=0,fail=0;

    while (child) numentries++,child=child->next;
        if (!numentries){
            out=(char*)json_malloc(fmt?depth+4:3);
            if (!out)
                return 0;
            ptr=out;*ptr++='{';
            if (fmt) {*ptr++='\n';for (i=0;i<depth-1;i++) *ptr++='\t';}
            *ptr++='}';*ptr++=0;
            return out;
    }

    entries=(char**)json_malloc(numentries*sizeof(char*));
    if (!entries) return 0;
    names=(char**)json_malloc(numentries*sizeof(char*));
    if (!names) {json_free(entries);return 0;}
    memset(entries,0,sizeof(char*)*numentries);
    memset(names,0,sizeof(char*)*numentries);

    child=item->child;depth++;if (fmt) len+=depth;
    while (child){
        names[i]=str=print_string_ptr(child->string);
        entries[i++]=ret=print_value(child,depth,fmt);
        if (str && ret) len+=strlen(ret)+strlen(str)+2+(fmt?2+depth:0); else fail=1;
            child=child->next;
    }

    if (!fail) out=(char*)json_malloc(len);
    if (!out) fail=1;

    if (fail){
        for (i=0;i<numentries;i++) {if (names[i]) json_free(names[i]);if (entries[i]) json_free(entries[i]);}
        json_free(names);json_free(entries);
            return 0;
    }

    *out='{';ptr=out+1;if (fmt)*ptr++='\n';*ptr=0;
    for (i=0;i<numentries;i++){
        if (fmt){
            for (j=0;j<depth;j++){
                *ptr++='\t';
            }
        }

        strcpy(ptr,names[i]);ptr+=strlen(names[i]);
        *ptr++=':';if (fmt) *ptr++='\t';
        strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
        if (i!=numentries-1) *ptr++=',';
        if (fmt) {*ptr++='\n';}
		*ptr=0;
        json_free(names[i]);json_free(entries[i]);
    }

    json_free(names);json_free(entries);
    if (fmt) for (i=0;i<depth-1;i++) *ptr++='\t';
    *ptr++='}';*ptr++=0;
    return out;
}

int    json_get_array_size(Json *array){
    Json *c=array->child;
    int i=0;
    while(c)
        i++,c=c->next;
    return i;
}

Json *json_get_array_item(Json *array,int item){
    Json *c=array->child;
    while (c && item>0)
        item--,c=c->next;

    return c;
}

Json *json_get_object_item(Json *object,const char *string){
    Json *c=object->child;
    while (c && json_strcasecmp(c->string,string))
        c=c->next;
    return c;
}

static void suffix_object(Json *prev,Json *item){
    prev->next=item;
    item->prev=prev;
}

static Json *create_reference(Json *item) {
    Json *ref=json_new_item();
    if (!ref) return 0;
    memcpy(ref,item,sizeof(Json));
    ref->string=0;
    ref->type|=json_is_reference;
    ref->next=ref->prev=0;
    return ref;
}

void json_add_item_to_array(Json *array, Json *item){
    Json *c=array->child;
    if (!item) return;

    if (!c) {
        array->child=item;
    }else {
        while (c && c->next)
            c=c->next;
        suffix_object(c,item);
    }
}

void json_add_item_to_object(Json *object,const char *string,Json *item){
    if (!item){
        return;
    }

    if (item->string){
        json_free(item->string);
    }

    item->string=json_strdup(string);
    json_add_item_to_array(object,item);
}

void json_add_item_reference_to_array(Json *array, Json *item){
    json_add_item_to_array(array,create_reference(item));
}

void json_add_item_reference_to_object(Json *object,const char *string,Json *item){
    json_add_item_to_object(object,string,create_reference(item));
}

Json *json_detach_item_from_array(Json *array,int which){
    Json *c=array->child;

    while (c && which>0)
        c=c->next,which--;

    if (!c)
        return 0;

    if (c->prev)
         c->prev->next=c->next;

    if (c->next)
        c->next->prev=c->prev;

    if (c==array->child)
        array->child=c->next;

    c->prev=c->next=0;
    return c;
}

void json_delete_item_from_array(Json *array,int which){
    json_delete(json_detach_item_from_array(array,which));
}

Json *json_detach_item_from_object(Json *object,const char *string) {
    int i=0;
    Json *c=object->child;
    while (c && json_strcasecmp(c->string,string))
        i++,c=c->next;

    if (c) return json_detach_item_from_array(object,i);

    return 0;
}

void json_delete_item_from_object(Json *object,const char *string) {
    json_delete(json_detach_item_from_object(object,string));
}

void json_replace_item_in_array(Json *array,int which,Json *newitem){
    Json *c=array->child;
    while (c && which>0){
        c=c->next,which--;
    }

    if (!c) return;
    newitem->next=c->next;
    newitem->prev=c->prev;
    if (newitem->next)
        newitem->next->prev=newitem;

    if (c==array->child)
        array->child=newitem;
    else
        newitem->prev->next=newitem;

    c->next=c->prev=0;
    json_delete(c);
}

void json_replace_item_in_object(Json *object,const char *string,Json *newitem){
    int i=0;
    Json *c=object->child;
    while(c && json_strcasecmp(c->string,string))
        i++,c=c->next;

    if(c){
        newitem->string=json_strdup(string);
        json_replace_item_in_array(object,i,newitem);
    }
}

Json *json_create_null(void){
    Json *item=json_new_item();
    if(item)item->type=json_null;
    return item;
}

Json *json_create_true(void){
    Json *item=json_new_item();
    if(item)item->type=json_true;
    return item;
}

Json *json_create_false(void){
    Json *item=json_new_item();
    if(item)item->type=json_false;
    return item;
}

Json *json_create_bool(int b){
    Json *item=json_new_item();
    if(item)item->type=b?json_true:json_false;
    return item;
}

Json *json_create_number(double num){
    Json *item=json_new_item();
    if(item){item->type=json_number;item->valuedouble=num;item->valueint=(int)num;}
    return item;
}

Json *json_create_string(const char *string){
    Json *item=json_new_item();
    if(item){item->type=json_string;item->valuestring=json_strdup(string);}
    return item;
}

Json *json_create_array(void){
    Json *item=json_new_item();
    if(item)item->type=json_array;
    return item;
}

Json *json_create_object(void){
    Json *item=json_new_item();
    if(item)item->type=json_object;
    return item;
}

Json *json_create_int_array(const int *numbers,int count){
    int i;
    Json *n=0,*p=0,*a=json_create_array();

    for(i=0;a && i<count;i++){
        n=json_create_number(numbers[i]);
        if(!i)
            a->child=n;
        else
            suffix_object(p,n);

        p=n;
    }

    return a;
}

Json *json_create_float_array(const float *numbers,int count){
    int i;
    Json *n=0,*p=0,*a=json_create_array();
    for(i=0;a && i<count;i++){
        n=json_create_number(numbers[i]);
        if(!i)
            a->child=n;
        else
            suffix_object(p,n);
        p=n;
    }
    return a;
}

Json *json_create_double_array(const double *numbers,int count){
    int i;
    Json *n=0,*p=0,*a=json_create_array();

    for(i=0;a && i<count;i++){
        n=json_create_number(numbers[i]);
        if(!i)
            a->child=n;
        else
            suffix_object(p,n);

		p=n;
    }
    return a;
}

Json *json_create_string_array(const char **strings,int count){
    int i;
    Json *n=0,*p=0,*a=json_create_array();
    for(i=0;a && i<count;i++){
        n=json_create_string(strings[i]);
        if(!i)a->child=n;
        else suffix_object(p,n);
        p=n;
    }
    return a;
}

Json *json_duplicate(Json *item,int recurse){
    Json *newitem,*cptr,*nptr=0,*newchild;

    if (!item) return 0;

    newitem=json_new_item();
    if (!newitem) return 0;

    newitem->type=item->type&(~json_is_reference),newitem->valueint=item->valueint,newitem->valuedouble=item->valuedouble;
    if (item->valuestring){
        newitem->valuestring=json_strdup(item->valuestring);
        if (!newitem->valuestring){
            json_delete(newitem);return 0;
        }
    }
    if (item->string){
        newitem->string=json_strdup(item->string);
        if (!newitem->string){
            json_delete(newitem);return 0;
        }
    }

    if (!recurse) return newitem;

    cptr=item->child;
    while (cptr){
        newchild=json_duplicate(cptr,1);
        if (!newchild){
            json_delete(newitem);return 0;
        }

        if (nptr){
            nptr->next=newchild,newchild->prev=nptr;nptr=newchild;
        }else{
            newitem->child=newchild;nptr=newchild;
        }

        cptr=cptr->next;
    }

    return newitem;
}

void json_minify(char *json){
    char *into=json;
    while (*json){
        if (*json==' ') json++;
        else if (*json=='\t') json++;
        else if (*json=='\r') json++;
        else if (*json=='\n') json++;
        else if (*json=='/' && json[1]=='/')  while (*json && *json!='\n') json++;
        else if (*json=='/' && json[1]=='*') {
            while (*json && !(*json=='*' && json[1]=='/')){
                json++;
                json+=2;
            }
        }
        else if (*json=='\"'){
            *into++=*json++;
            while (*json && *json!='\"'){
                if (*json=='\\')
                    *into++=*json++;
                *into++=*json++;
            }

            *into++=*json++;
        }else *into++=*json++;
    }

    *into=0;
}
