import * as utils from 'bigint-crypto-utils';
import fs from 'fs';

const bitLength = 16;
const encrypted_path = 'encrypted/encrypted.txt';

async function generateNPQ(bitLength) {
    const p = await utils.prime(bitLength);
    const q = await utils.prime(bitLength);
    const n = p * q;
    return [n, p, q];
}

async function encrypt(message, e, n) {
    return await utils.modPow(message, e, n);
}

async function decrypt(encryptedMessage, d, n) {
    return await utils.modPow(encryptedMessage, d, n);
}

const phi = (p, q) => (p - 1n) * (q - 1n);

const message = await utils.prime(bitLength);
let e = 65537n;
const [n, p, q] = await generateNPQ(bitLength);
const numPhi = phi(p, q);

while (utils.gcd(e, numPhi) !== 1n) {
    e = await utils.prime(bitLength);
}

const d = await utils.modInv(e, numPhi);
const encryptedMessage = await encrypt(message, e, n);
const decryptedMessage = await decrypt(encryptedMessage, d, n)

console.log(
    `Public key: (${e}, ${n})\n` + 
    `Private key: (${d}, ${n})\n` +
    `Message: ${message}\n` +
    `Encrypted message: ${encryptedMessage}\n` +
    `Decrypted message: ${decryptedMessage}\n` +
    `Message encrypted in txt file by path: ${encrypted_path}\n` +
    `Is initial message = decrypted message? : ${decryptedMessage === message} `
);

fs.writeFileSync(encrypted_path, encryptedMessage.toString());