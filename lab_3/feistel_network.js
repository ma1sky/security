import fs from 'fs';
import crypto from 'crypto';

const in_path = 'in/George_Orwell_1984.txt';
const encrypted_path = 'encrypted/encrypted.bin';
const decrypted_path = 'decrypted/decrypted.txt';
const rounds = 10;
const chunkSize = 32;

const keys = createKeys(rounds, chunkSize);

function createKeys(rounds, chunkSize) {
    let keys = [];
    let randomValue;
    for (let i = 0; i < rounds; i++) {
        randomValue = crypto.randomBytes(chunkSize);
        keys.push(randomValue);
    }
    return keys;
}

function encrypt(data, keys) {
    const chunkSize = keys[0].length;
    const paddingLength = chunkSize - (data.length % chunkSize);
    if (paddingLength !== 0) {
        const padding = Buffer.alloc(paddingLength, paddingLength);
        data = Buffer.concat([data, padding]);
    }
    

    let left = data.slice(0, data.length / 2);
    let right = data.slice(data.length / 2);
    let temp;

    keys.forEach(key => {
        temp = right;    
        right = xor(left, xor(right, key));
        left = temp;
    })

    return [Buffer.concat([left, right]), paddingLength];
}


function decrypt(data, keys, paddingLength) {

    let left = data.slice(0, data.length / 2);
    let right = data.slice(data.length / 2);
    let temp;

    keys.reverse().forEach(key => {
        temp = left;    
        left = xor(right, xor(left, key));
        right = temp;
    })

    return Buffer.concat([left, right]).slice(0, data.length - paddingLength);
}

function xor(chunk, key) {
    let result = Buffer.alloc(chunk.length)
    for (let i = 0; i < chunk.length; i++) {
        result[i] = chunk[i] ^ key[i % key.length];
    }

    return result;
}

let encryptedData;
let paddingLength;

if (fs.existsSync(in_path)) {
    try {
        const data = fs.readFileSync(in_path);
        [encryptedData, paddingLength] = encrypt(data, keys);
        fs.writeFileSync(encrypted_path, encryptedData);

        encryptedData = fs.readFileSync(encrypted_path);

        let decryptedData = decrypt(encryptedData, keys, paddingLength);
        fs.writeFileSync(decrypted_path, decryptedData);

    } catch(err) {
        console.error(err);
    }
} else {
    console.error('File doesnt exists!');
}