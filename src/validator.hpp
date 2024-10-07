#ifndef VALIDATOR_H
#define VALIDATOR_H

/*******************************************************************************
**                                  INCLUDES                                  **
*******************************************************************************/
#include <stdint.h>
#include <stdio.h>

/*******************************************************************************
**                                 FUNCTIONS                                  **
*******************************************************************************/
bool json_valid_buff(char *b, uint_fast64_t len, bool is_dict);
bool json_valid(FILE *f);

#endif // !VALIDATOR_H