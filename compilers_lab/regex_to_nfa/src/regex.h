#ifndef REGEX_H
#define REGEX_H

typedef struct
{
    char value;
} regex_item;

typedef struct
{
    regex_item *items;
    int size;
} regex;

regex parse_regex(const char *str);

#endif
