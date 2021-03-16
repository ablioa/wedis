#ifndef json__h
#define json__h

#define json_false  0
#define json_true   1
#define json_null   2
#define json_number 3 
#define json_string 4
#define json_array  5
#define json_object 6

#define json_is_reference 256

typedef struct Json {
	struct Json *next,*prev;
	struct Json *child;

	int type;

	char *valuestring;
	int valueint;
	double valuedouble;

	char *string;
} Json;

typedef struct json_hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} json_hooks;

extern void json_init_hooks(json_hooks * hooks);

extern Json  * json_parse(const char *value);
extern char  * json_print(Json *item);
extern char  * json_print_unformatted(Json * item);
extern void    json_delete(Json *c);

extern int   json_get_array_size(Json *array);
extern Json *json_get_array_item(Json *array,int item);
extern Json *json_get_object_item(Json *object,const char *string);

extern const char * json_get_error_ptr(void);

extern Json *json_create_null(void);
extern Json *json_create_true(void);
extern Json *json_create_false(void);
extern Json *json_create_bool(int b);
extern Json *json_create_number(double num);
extern Json *json_create_string(const char *string);
extern Json *json_create_array(void);
extern Json *json_create_object(void);

extern Json *json_create_int_array(const int *numbers,int count);
extern Json *json_create_float_array(const float *numbers,int count);
extern Json *json_create_double_array(const double *numbers,int count);
extern Json *json_create_string_array(const char **strings,int count);

extern void json_add_item_to_array(Json *array, Json *item);
extern void json_add_item_to_object(Json *object,const char *string,Json *item);
extern void json_add_item_reference_to_array(Json *array, Json *item);
extern void json_add_item_reference_to_object(Json *object,const char *string,Json *item);

extern Json *json_detach_item_from_array(Json *array,int which);
extern void   json_delete_item_from_array(Json *array,int which);
extern Json *json_detach_item_from_object(Json *object,const char *string);
extern void   json_delete_item_from_object(Json *object,const char *string);

extern void json_replace_item_in_array(Json *array,int which,Json *newitem);
extern void json_replace_item_in_object(Json *object,const char *string,Json *newitem);

extern Json *json_duplicate(Json *item,int recurse);
extern Json *json_parse_with_opts(const char *value,const char **return_parse_end,int require_null_terminated);

extern void json_minify(char *json);

#define json_add_null_to_object(object,name)     json_add_item_to_object(object, name, json_create_null())
#define json_add_true_to_object(object,name)     json_add_item_to_object(object, name, json_create_true())
#define json_add_false_to_object(object,name)    json_add_item_to_object(object, name, json_create_false())
#define json_add_bool_to_object(object,name,b)   json_add_item_to_object(object, name, json_create_bool(b))
#define json_add_number_to_object(object,name,n) json_add_item_to_object(object, name, json_create_number(n))
#define json_add_string_to_object(object,name,s) json_add_item_to_object(object, name, json_create_string(s))

#define json_set_int_value(object,val) ((object)?(object)->valueint=(object)->valuedouble=(val):(val))

#endif
