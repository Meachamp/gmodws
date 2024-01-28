#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <iostream>
#include <cstdint>
#include <cstring>
#include <time.h>
#include <endian.h>
#include "2fa.h"

uint64_t get_time() {
    return time(0);
}

uint8_t* hmac_sha1(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len) {
    uint8_t* result = HMAC(EVP_sha1(), key, key_len, data, data_len, NULL, NULL);
    return result;
}

struct base64_result_t {
    uint8_t* data;
    size_t len;
};

base64_result_t b64_decode(const char* input) {
    size_t input_len = strlen(input);
    size_t output_predicted = input_len/4 * 3;
    unsigned char* output_data = new unsigned char[output_predicted]();
    if(!output_data) {
        return {};
    }

    EVP_ENCODE_CTX* ctx = EVP_ENCODE_CTX_new();
    EVP_DecodeInit(ctx);

    int output_len = 0;
    if (EVP_DecodeUpdate(ctx, output_data, &output_len, (const unsigned char*)input, input_len) == -1) {
        EVP_ENCODE_CTX_free(ctx);
        delete[] output_data;
        return {};
    }

    int final_written = 0;
    if(EVP_DecodeFinal(ctx, output_data, &final_written) == -1) {
        EVP_ENCODE_CTX_free(ctx);
        delete[] output_data;
        return {};
    }

    output_len += final_written;
    EVP_ENCODE_CTX_free(ctx);

    base64_result_t ret = {};
    ret.data = output_data;
    ret.len = output_len;

    return ret;
}

uint8_t symbols[26] = {50, 51, 52, 53, 54, 55, 56, 57, 66, 67, 68, 70, 71, 72, 74, 75, 77, 78, 80, 81, 82, 84, 86, 87, 88, 89};
#define NUM_SYMBOLS 26

char* compute_code(const uint8_t* secret, size_t secret_len) {
    uint64_t t = get_time();
    t /= 30;
    t = htobe64(t);

    uint8_t* hmac = hmac_sha1(secret, secret_len, (uint8_t*)&t, sizeof(t));
    if (!hmac) return NULL;

    size_t idx = hmac[19] & 0xF;
    int code_point = (hmac[idx] & 0x7F) << 24 | hmac[idx+1] << 16 | hmac[idx+2] << 8 | hmac[idx+3];

    char* code = new char[6]();

    for(int i = 0; i < 5; i++) {
        code[i] = symbols[code_point % NUM_SYMBOLS];
        code_point /= NUM_SYMBOLS;
    }

    return code;
}

/* INPUT: Base64 encoded shared secret as const char*
   OUTPUT: 2FA Code as char* (or NULL on failure)
*/
char* code_from_secret(const char* shared_secret) {
    base64_result_t secret = b64_decode(shared_secret);

    if(!secret.data) {
        return NULL;
    }

    char* code = compute_code(secret.data, secret.len);
    delete[] secret.data;
    secret = {};

    return code;
}
