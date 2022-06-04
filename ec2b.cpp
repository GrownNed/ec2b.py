#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include <random>

#include "magic.h"

// These functions are not exported, so hackaround it
extern "C" void oqs_mhy128_enc_c(const uint8_t *plaintext, const void *_schedule, uint8_t *ciphertext);
extern "C" void oqs_mhy128_dec_c(const uint8_t *chiphertext, const void *_schedule, uint8_t *plaintext);

// UnityPlayer:$26EA90
void key_scramble(uint8_t* key) {
    uint8_t round_keys[11*16] = {0};
    for (int round = 0; round <= 10; round++) {
        for (int i = 0; i < 16; i++) {
            for (int j = 0; j < 16; j++) {
                uint64_t idx = (round << 8) + (i*16) + j;
                round_keys[round * 16 + i] ^= aes_xorpad_table[1][idx] ^ aes_xorpad_table[0][idx];
            }
        }
    }

    uint8_t chip[16];
    oqs_mhy128_enc_c(key, round_keys, chip);
    memcpy(key, chip, 16);
}

// UnityPlayer:$19DA40
void get_decrypt_vector(uint8_t* key, uint8_t* crypt, uint64_t crypt_size, uint8_t* output, uint64_t output_size) {
    uint64_t val = 0xFFFFFFFFFFFFFFFF;
    for (int i = 0; i < crypt_size >> 3; i++) {
        val = ((uint64_t*)crypt)[i] ^ val;
    }

    auto* key_qword = (uint64_t*)key;
    auto mt = std::mt19937_64(key_qword[1] ^ 0xCEAC3B5A867837AC ^ val ^ key_qword[0]);
    for (uint64_t i = 0; i < output_size >> 3; i++) {
        ((uint64_t*)output)[i] = mt();
    }
}

static PyObject* derive(PyObject* self, PyObject* args)
{
    uint8_t* ec2b;
    Py_ssize_t size;

    if (!PyArg_ParseTuple(args, "y#", &ec2b, &size))
        return NULL;

    if (size != 2076)
        return PyErr_Format(PyExc_ValueError, "ec2b size must be 2076 (got %d)", size);

    uint8_t key[16];
    uint8_t data[2048];

    memcpy(key, &ec2b[8], sizeof(key));
    memcpy(data, &ec2b[28], sizeof(data));

    key_scramble(key);
    for (int i = 0; i < 16; i++) {
        key[i] ^= key_xorpad_table[i];
    }

    uint8_t xorpad[4096];
    get_decrypt_vector(key, data, sizeof(data), xorpad, sizeof(xorpad));

    return Py_BuildValue("y#", xorpad, sizeof(xorpad));
}

static PyMethodDef methods[] = {
    { "derive", derive, METH_VARARGS, "Derive key from ec2b" },
    { NULL, NULL, 0, NULL }
};

static struct PyModuleDef module = {
    PyModuleDef_HEAD_INIT,
    "ec2b",
    "Module for ec2b encryption",
    -1,
    methods
};

PyMODINIT_FUNC PyInit_ec2b(void)
{
    return PyModule_Create(&module);
}
