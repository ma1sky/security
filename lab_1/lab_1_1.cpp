#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

size_t getLen(const string& filepath) {
    ifstream in(filepath, ios::binary | ios::ate);
    return in ? static_cast<size_t>(in.tellg()) : 0;
}

vector<pair<int, size_t>> sortFreqs(const vector<size_t>& freqs) {
    vector<pair<int, size_t>> result;

    for (int i = 0; i < 256; i++) {
        if (freqs[i] > 0) {
            result.push_back({i, freqs[i]});
        }
    }

    sort(result.begin(), result.end(),
         [](const pair<int, size_t>& a, const pair<int, size_t>& b) {
             return a.second > b.second;
         });

    return result;
}

void writeLog(const vector<pair<int, size_t>>& freqs,
              const string& filepath,
              const string& keypath) {

    ofstream out("log.md");
    if (!out) return;

    size_t filelen = getLen(filepath);
    if (filelen == 0) {
        out << "File is empty\n";
        return;
    }

    out << "| Byte | Count | Frequency |\n";
    out << "|------|-------|-----------|\n";

    for (const auto& p : freqs) {
        double percent = 100.0 * p.second / filelen;
        out << "| " << p.first
            << " | " << p.second
            << " | " << percent << "% |\n";
    }
}

vector<int> readKey(const string& path) {
    ifstream in(path);
    if (!in) throw runtime_error("Cannot open key file");

    vector<int> key(256);

    for (int i = 0; i < 256; i++) {
        if (!(in >> key[i]))
            throw runtime_error("Key must contain 256 numbers");

        if (key[i] < 0 || key[i] > 255)
            throw runtime_error("Key values must be 0..255");
    }

    return key;
}

vector<int> buildInverseKey(const vector<int>& key) {
    vector<int> inverse(256);
    vector<bool> used(256, false);

    for (int i = 0; i < 256; i++) {
        int value = key[i];

        if (used[value])
            throw runtime_error("Key must be a permutation");

        used[value] = true;
        inverse[value] = i;
    }

    return inverse;
}

vector<char> readFile(const string& path, vector<size_t>& freqs) {
    ifstream in(path, ios::binary);
    if (!in) throw runtime_error("Cannot open input file");

    in.seekg(0, ios::end);
    size_t size = in.tellg();
    in.seekg(0, ios::beg);

    vector<char> data(size);
    in.read(data.data(), size);

    for (char c : data) {
        freqs[(unsigned char)c]++;
    }

    return data;
}

void transform(vector<char>& data, const vector<int>& table) {
    for (size_t i = 0; i < data.size(); i++) {
        unsigned char byte = data[i];
        data[i] = table[byte];
    }
}

void writeFile(const string& path, const vector<char>& data) {
    ofstream out(path, ios::binary);
    if (!out) throw runtime_error("Cannot create output file");

    out.write(data.data(), data.size());
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        cout << "Usage: program input key -C|-D\n";
        return 1;
    }

    string filepath = argv[1];
    string keypath  = argv[2];
    string mode     = argv[3];

    try {
        cout << "File size: " << getLen(filepath) << " bytes\n";

        vector<size_t> freqs(256, 0);
        vector<char> data = readFile(filepath, freqs);

        vector<int> key = readKey(keypath);

        if (mode == "-D")
            key = buildInverseKey(key);

        transform(data, key);

        string outName = (mode == "-C") ?
                         "encrypted.bin" :
                         "decrypted.docx";

        writeFile(outName, data);

        cout << "Output: " << outName << endl;

        writeLog(sortFreqs(freqs), filepath, keypath);

    } catch (exception& e) {
        cout << e.what() << endl;
        return 1;
    }

    return 0;
}
