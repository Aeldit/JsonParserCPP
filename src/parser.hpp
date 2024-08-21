#ifndef JSON_PARSER_H
#define JSON_PARSER_H

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stdint.h>
#include <stdio.h>

#include "json.hpp"

/*******************************************************************************
**                                 FUNCTIONS                                  **
*******************************************************************************/
JSONDict *parse_json_dict(FILE *f, uint64_t *pos);

JSONDict *parse(char *file);

#endif // !JSON_PARSER_H
