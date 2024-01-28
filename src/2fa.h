#pragma once

/* INPUT: Base64 encoded shared secret as const char*
   OUTPUT: 2FA Code as char* (or NULL on failure)
*/
char* code_from_secret(const char* shared_secret);