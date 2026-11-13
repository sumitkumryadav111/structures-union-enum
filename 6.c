
#include <stdio.h>
#include <string.h>
#include <stddef.h>     
#include <stdalign.h>   

#define TEXT_SIZE 20    // 19 letters + 1 for the end mark '\0'

typedef union {
    int i;
    float f;
    char str[TEXT_SIZE];
} UnionData;

typedef struct {
    int i;
    float f;
    char str[TEXT_SIZE];
} StructData;

// The tag: tells which union member is in use
enum ValueType { VALUE_INT, VALUE_FLOAT, VALUE_TEXT };

typedef struct {
    enum ValueType type;
    UnionData value;
} TaggedValue;

//  functions
// Store an int and set the tag
void set_int(TaggedValue *t, int number) {
    t->value.i = number;
    t->type = VALUE_INT;
}

// Store a float and set the tag
void set_float(TaggedValue *t, float number) {
    t->value.f = number;
    t->type = VALUE_FLOAT;
}

// Store text and set the tag. Returns 1 if ok, 0 if the text is too long.
int set_text(TaggedValue *t, const char *text) {
    if (strlen(text) >= TEXT_SIZE) {
        printf("Error: \"%s\" is too long (maximum %d letters).\n", text, TEXT_SIZE - 1);
        return 0;
    }
    strcpy(t->value.str, text);
    t->type = VALUE_TEXT;
    return 1;
}

// Print only the member that the tag says is in use
void print_value(TaggedValue *t) {
    if (t->type == VALUE_INT) {
        printf("int   = %d\n", t->value.i);
    } else if (t->type == VALUE_FLOAT) {
        printf("float = %.2f\n", t->value.f);
    } else if (t->type == VALUE_TEXT) {
        printf("text  = %s\n", t->value.str);
    } else {
        printf("Error: bad tag (%d). Nothing printed.\n", (int)t->type);
    }
}

int main() {
    UnionData udata;
    StructData sdata;

    // ----- 1. Give values -----
    printf("--- 1. Give values ---\n");

    sdata.i = 10;
    sdata.f = 220.5;
    strcpy(sdata.str, "Struct Programming");
    printf("Struct (all three are kept):\n");
    printf("  %d, %.2f, %s\n", sdata.i, sdata.f, sdata.str);

    printf("Union (print right after each value is stored):\n");
    udata.i = 10;
    printf("  i = %d\n", udata.i);
    udata.f = 220.5;
    printf("  f = %.2f\n", udata.f);
    strcpy(udata.str, "Union Programming");
    printf("  str = %s\n", udata.str);

    // 2. Size and alignment
        printf("\n--- 2. Size and alignment ---\n");
    printf("UnionData  : size = %zu, alignment = %zu\n", sizeof(UnionData), alignof(UnionData));
    printf("StructData : size = %zu, alignment = %zu\n", sizeof(StructData), alignof(StructData));

    // 3. Addresses 
    printf("\n--- 3. Addresses ---\n");
    printf("Union members (all same address):\n");
    printf("  i   : %p\n", (void *)&udata.i);
    printf("  f   : %p\n", (void *)&udata.f);
    printf("  str : %p\n", (void *)&udata.str);
    printf("Struct members (all different addresses):\n");
    printf("  i   : %p\n", (void *)&sdata.i);
    printf("  f   : %p\n", (void *)&sdata.f);
    printf("  str : %p\n", (void *)&sdata.str);

    // 4. Offsets
    printf("\n--- 4. Struct offsets (bytes from the start) ---\n");
    printf("i   : %zu\n", offsetof(StructData, i));
    printf("f   : %zu\n", offsetof(StructData, f));
    printf("str : %zu\n", offsetof(StructData, str));
    printf("Each member starts at a place that fits its alignment.\n");
    printf("If it does not fit, the compiler adds empty bytes (padding).\n");

    // 5. Memory for 100 records
    printf("\n--- 5. Memory for 100 records ---\n");
    printf("100 structs : %zu bytes\n", 100 * sizeof(StructData));
    printf("100 unions  : %zu bytes\n", 100 * sizeof(UnionData));
    printf("100 tagged  : %zu bytes\n", 100 * sizeof(TaggedValue));
    printf("Size of one tagged record = %zu bytes\n", sizeof(TaggedValue));

    // 6. Five tagged records
        printf("\n--- 6. Five tagged records ---\n");
    TaggedValue records[5];
    set_int(&records[0], 10);
    set_float(&records[1], 220.5);
    set_text(&records[2], "Union Programming");
    set_int(&records[3], 42);
    set_text(&records[4], "Hello");

    for (int i = 0; i < 5; i++) {
        printf("Record %d: ", i + 1);
        print_value(&records[i]);
    }

    // 7. Edge cases 
    printf("\n--- 7. Edge cases ---\n");

    printf("Text with 19 letters (biggest that fits):\n  ");
    if (set_text(&records[3], "1234567890123456789")) {
        print_value(&records[3]);
    }

    printf("Text with 20 letters (too long):\n  ");
    set_text(&records[4], "12345678901234567890");
    printf("  Record 5 is still: ");
    print_value(&records[4]);

    printf("One record: int, then text, then float:\n");
    TaggedValue r;
    set_int(&r, 7);
    printf("  "); print_value(&r);
    set_text(&r, "seven");
    printf("  "); print_value(&r);
    set_float(&r, 7.5);
    printf("  "); print_value(&r);

    printf("Bad tag:\n  ");
    r.type = (enum ValueType)99;
    print_value(&r);

    
    printf("\n--- Notes ---\n");
    printf("1. Union members share the same space, so they start at the same address.\n");
    printf("   Storing one value wipes out the other values.\n");
    printf("2. Struct members each have their own space, so all values stay.\n");
    printf("3. A union is only as big as its biggest member.\n");
    printf("   A struct is as big as all members together (plus padding).\n");
    printf("4. A union saves memory when a record needs only ONE kind of value\n");
    printf("   at a time. But the values cannot live together.\n");
    printf("5. The tag tells us which member is safe to read.\n");

    return 0;
}
