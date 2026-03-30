#include <iostream>
#include <openssl/bn.h>
#include <openssl/rand.h>
using namespace std;

// Преобразование строки в BIGNUM
BIGNUM* stringToBN(const string& text) {
    BIGNUM* bn = BN_new();
    BN_zero(bn);

    for (unsigned char c : text) {
        BN_lshift(bn, bn, 8); // << 8
        BN_add_word(bn, c);   // + c
    }
    return bn;
}

// Преобразование BIGNUM в строку
string bnToString(BIGNUM* bn) {
    string result;
    BIGNUM* tmp = BN_dup(bn);
    BIGNUM* zero = BN_new();
    BN_zero(zero);

    while (BN_cmp(tmp, zero) > 0) {
        unsigned long c = BN_mod_word(tmp, 256);
        result = static_cast<char>(c) + result;
        BN_rshift(tmp, tmp, 8);
    }

    BN_free(tmp);
    BN_free(zero);
    return result;
}

// Генерация случайного простого числа заданной битности
BIGNUM* generatePrimeBN(int bits) {
    BIGNUM* prime = BN_new();
    BN_generate_prime_ex(prime, bits, 1, nullptr, nullptr, nullptr);
    return prime;
}

int main(int argc, char* argv[]) {

    if (argc != 5 && argc != 3) {
        cout << "Usage: program <-C|-D> <m> <_|d> <_|n>\n";
        return 1;
    }

    string mode = argv[1];
    BIGNUM* message;
    BIGNUM* publicKey = nullptr;
    BIGNUM* privateKey = nullptr;
    BIGNUM* n = nullptr;
    BN_CTX* ctx = BN_CTX_new();

    if (mode == "-D") {
        message = BN_new();
        privateKey = BN_new();
        n = BN_new();

        BN_dec2bn(&message, argv[2]);
        BN_dec2bn(&privateKey, argv[3]);
        BN_dec2bn(&n, argv[4]);

        BIGNUM* decrypted = BN_new();
        BN_mod_exp(decrypted, message, privateKey, n, ctx);

        string result = bnToString(decrypted);
        cout << "Decrypted: " << result << endl;

        BN_free(message);
        BN_free(privateKey);
        BN_free(n);
        BN_free(decrypted);

    } else if (mode == "-C") {
        message = stringToBN(argv[2]);

        BIGNUM* p = generatePrimeBN(512);
        BIGNUM* q = generatePrimeBN(512);

        n = BN_new();
        BN_mul(n, p, q, ctx);

        // phi = (p-1)*(q-1)
        BIGNUM* phi = BN_new();
        BN_sub_word(p, 1);
        BN_sub_word(q, 1);
        BN_mul(phi, p, q, ctx); 
        BN_add_word(p, 1);
        BN_add_word(q, 1);

        int coprime = 0;
        do {
            if (publicKey) BN_free(publicKey);
            publicKey = generatePrimeBN(256);

            BIGNUM* gcd = BN_new();
            BN_gcd(gcd, publicKey, phi, ctx);
            coprime = BN_is_one(gcd);
            BN_free(gcd);

        } while (!coprime);

        privateKey = BN_new();
        BN_mod_inverse(privateKey, publicKey, phi, ctx); // d = e^-1 mod phi

        BIGNUM* encryptedMessage = BN_new();
        BN_mod_exp(encryptedMessage, message, publicKey, n, ctx); // c = m^e mod n

        char* enc_str = BN_bn2dec(encryptedMessage);
        char* e_str   = BN_bn2dec(publicKey);
        char* d_str   = BN_bn2dec(privateKey);
        char* n_str   = BN_bn2dec(n);

        cout << "Encrypted: " << enc_str << endl;
        cout << "Public key (e, n): " << e_str << " " << n_str << endl;
        cout << "Private key (d, n): " << d_str << " " << n_str << endl;

        OPENSSL_free(enc_str);
        OPENSSL_free(e_str);
        OPENSSL_free(d_str);
        OPENSSL_free(n_str);

        BN_free(message);
        BN_free(p);
        BN_free(q);
        BN_free(n);
        BN_free(phi);
        BN_free(publicKey);
        BN_free(privateKey);
        BN_free(encryptedMessage);
    } else {
        cout << "Unexpected mode\n";
        return 1;
    }

    BN_CTX_free(ctx);
    return 0;
}