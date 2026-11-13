/*
 Problem 5: A typed container using enum, union and structure (C11).
 Sample: INT 10, FLOAT 3.14, CHAR A, STRING Hello; copied "Data"
 remains "Data" after the original changes.
 Tests: zero, wrong getter, empty value/container, allocation failure,
 STRING->INT->STRING->FLOAT, repeated clear, deep/self copy, same-pointer
 string replacement. Allocation failure is simulated by a test flag.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_EMPTY, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING } DataType;

typedef union {
    int i;
    float f;
    char ch;
    char *str;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

static int fail_allocation = 0;

static char *copy_text(const char *text)
{
    size_t len = strlen(text);
    char *copy;

    if (fail_allocation) {
        fail_allocation = 0;
        return NULL;
    }

    copy = malloc(len + 1);
    if (copy != NULL) {
        memcpy(copy, text, len + 1);
    }

    return copy;
}

void container_init(Container *c)
{
    if (c != NULL) {
        c->type = TYPE_EMPTY;
        c->data.str = NULL;
    }
}

void container_clear(Container *c)
{
    if (c == NULL) {
        return;
    }

    if (c->type == TYPE_STRING) {
        free(c->data.str);
    }

    c->type = TYPE_EMPTY;
    c->data.str = NULL;
}

int container_set_int(Container *c, int value)
{
    if (c == NULL) return 0;
    container_clear(c);
    c->data.i = value;
    c->type = TYPE_INT;
    return 1;
}

int container_set_float(Container *c, float value)
{
    if (c == NULL) return 0;
    container_clear(c);
    c->data.f = value;
    c->type = TYPE_FLOAT;
    return 1;
}

int container_set_char(Container *c, char value)
{
    if (c == NULL) return 0;
    container_clear(c);
    c->data.ch = value;
    c->type = TYPE_CHAR;
    return 1;
}

/* Allocate before clearing, so failure preserves the old value and
   passing the container's own string pointer is safe. */
int container_set_string(Container *c, const char *value)
{
    char *copy;

    if (c == NULL || value == NULL) return 0;
    copy = copy_text(value);
    if (copy == NULL) return 0;

    container_clear(c);
    c->data.str = copy;
    c->type = TYPE_STRING;
    return 1;
}

int container_get_int(const Container *c, int *out)
{
    if (c == NULL || out == NULL || c->type != TYPE_INT) return 0;
    *out = c->data.i;
    return 1;
}

int container_get_float(const Container *c, float *out)
{
    if (c == NULL || out == NULL || c->type != TYPE_FLOAT) return 0;
    *out = c->data.f;
    return 1;
}

int container_get_char(const Container *c, char *out)
{
    if (c == NULL || out == NULL || c->type != TYPE_CHAR) return 0;
    *out = c->data.ch;
    return 1;
}

int container_get_string(const Container *c, const char **out)
{
    if (c == NULL || out == NULL || c->type != TYPE_STRING) return 0;
    *out = c->data.str;
    return 1;
}

const char *container_type_name(const Container *c)
{
    if (c == NULL) return "INVALID";

    switch (c->type) {
        case TYPE_EMPTY: return "EMPTY";
        case TYPE_INT: return "INT";
        case TYPE_FLOAT: return "FLOAT";
        case TYPE_CHAR: return "CHAR";
        case TYPE_STRING: return "STRING";
        default: return "INVALID";
    }
}

void container_print(const Container *c)
{
    if (c == NULL) {
        printf("<invalid>");
        return;
    }

    switch (c->type) {
        case TYPE_EMPTY: printf("<empty>"); break;
        case TYPE_INT: printf("%d", c->data.i); break;
        case TYPE_FLOAT: printf("%.2f", c->data.f); break;
        case TYPE_CHAR: printf("%c", c->data.ch); break;
        case TYPE_STRING: printf("%s", c->data.str); break;
        default: printf("<invalid>"); break;
    }
}

/* Destination must be initialized. String values are copied independently. */
int container_copy(Container *dest, const Container *src)
{
    Container temp;

    if (dest == NULL || src == NULL) return 0;
    if (dest == src) return 1;

    container_init(&temp);

    switch (src->type) {
        case TYPE_EMPTY: break;
        case TYPE_INT:
            temp.type = TYPE_INT;
            temp.data.i = src->data.i;
            break;
        case TYPE_FLOAT:
            temp.type = TYPE_FLOAT;
            temp.data.f = src->data.f;
            break;
        case TYPE_CHAR:
            temp.type = TYPE_CHAR;
            temp.data.ch = src->data.ch;
            break;
        case TYPE_STRING:
            if (src->data.str == NULL ||
                !container_set_string(&temp, src->data.str)) return 0;
            break;
        default: return 0;
    }

    container_clear(dest);
    *dest = temp;
    return 1;
}

int main(void)
{
    Container records[7];
    int number;
    float decimal;
    const char *text;
    size_t i;

    for (i = 0; i < 7; i++) container_init(&records[i]);

    /* Original sequential demonstration using the same container. */
    container_set_int(&records[0], 10);
    printf("Int: "); container_print(&records[0]); printf("\n");

    container_set_float(&records[0], 3.14f);
    printf("Float: "); container_print(&records[0]); printf("\n");

    container_set_char(&records[0], 'A');
    printf("Char: "); container_print(&records[0]); printf("\n");

    container_set_string(&records[0], "Hello, World!");
    printf("String: "); container_print(&records[0]); printf("\n\n");

    /* Six mixed-type records and one independent copy. */
    container_set_int(&records[0], 10);
    container_set_float(&records[1], 3.14f);
    container_set_char(&records[2], 'A');
    container_set_string(&records[3], "Hello");
    container_set_int(&records[4], 0);
    container_set_string(&records[5], "Data");
    container_copy(&records[6], &records[5]);
    container_set_string(&records[5], "Changed");

    for (i = 0; i < 6; i++) {
        printf("[%zu] %s: ", i, container_type_name(&records[i]));
        container_print(&records[i]);
        printf("\n");
    }
    printf("Deep copy: "); container_print(&records[6]); printf("\n");

    /* Required edge cases. */
    container_clear(&records[0]);
    printf("Empty container: "); container_print(&records[0]); printf("\n");

    container_set_int(&records[0], 0);
    printf("Integer zero getter: %s\n",
           container_get_int(&records[0], &number) && number == 0 ? "PASS" : "FAIL");
    printf("Wrong-type getter: %s\n",
           container_get_float(&records[0], &decimal) ? "FAIL" : "PASS");

    container_set_string(&records[0], "");
    printf("Empty string: "); container_print(&records[0]); printf("\n");

    container_set_string(&records[0], "one");
    container_set_int(&records[0], 5);
    container_set_string(&records[0], "two");
    container_set_float(&records[0], 2.5f);
    printf("STRING->INT->STRING->FLOAT: %s\n",
           records[0].type == TYPE_FLOAT ? "PASS" : "FAIL");

    fail_allocation = 1;
    printf("Allocation failure preserves old value: %s\n",
           !container_set_string(&records[0], "fail") &&
           records[0].type == TYPE_FLOAT ? "PASS" : "FAIL");

    container_set_string(&records[0], "same");
    container_get_string(&records[0], &text);
    printf("Same-pointer replacement: %s\n",
           container_set_string(&records[0], text) ? "PASS" : "FAIL");
    printf("Self-copy: %s\n",
           container_copy(&records[0], &records[0]) ? "PASS" : "FAIL");

    for (i = 0; i < 7; i++) {
        container_clear(&records[i]);
        container_clear(&records[i]); /* Repeated clear is safe. */
    }

    return 0;
}
