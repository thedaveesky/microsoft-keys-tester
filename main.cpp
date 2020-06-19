#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <random>
#include <algorithm>
#include <ctime>

using namespace std;

const string dict = "BCDFGHJKMPQRTVWXY2346789";
unsigned int table[256];

mt19937 mt(time(NULL));
uniform_int_distribution<unsigned long long> ato, ipk(0, 0x3305cab);

// Function keygen from https://github.com/wegank/claymore
string keygen()
{
    string hash;
    unsigned long long atoid = ato(mt), ipkid = ipk(mt), refid = 0xCBC, crc32 = 0xffffffff, temp,
            data[2] = {(atoid & 0x3f) << 50 | (ipkid & 0x3fffffff) << 20 | (refid & 0xfffff), (atoid >> 6) & 0x7fffffffffff};

    for (int i = 0; i < 7; ++i) crc32 = (crc32 << 8) ^ table[((data[0] >> 8 * i) ^ (crc32 >> 24)) & 0xff];
    for (int i = 0; i < 8; ++i) crc32 = (crc32 << 8) ^ table[((data[1] >> 8 * i) ^ (crc32 >> 24)) & 0xff];
    data[1] |= (~((crc32 << 8) ^ table[(crc32 >> 24) & 0xff]) & 0x3ff) << 47;

    for (int i = 0; i < 24; ++i)
    {
        temp = (data[1] % 24) << 56 | data[0];
        hash += dict[temp % 24];
        data[0] = temp / 24;
        data[1] /= 24;
    }

    reverse(hash.begin(), hash.end());
    hash.insert(((data[1] % 24) << 56 | data[0]) % 24, "N");
    for (int i = 20; i > 0; i -= 5) hash.insert(i, "-");
    return hash;
}

// Function exec from https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main() {
	for (int i = 0; i < 256; ++i) {
        unsigned temp = i << 24;
        for (int j = 0; j < 8; ++j) temp = temp & 0x80000000 ? (temp << 1) ^ 0x04c11db7 : (temp << 1);
        table[i] = temp;
    }
    while (true) {
    string key = keygen();
    string install = string("cscript //nologo C:\\Windows\\system32\\slmgr.vbs /ipk ")+key;
    std::string s1 = exec(install.c_str());
	std::string s = exec("cscript //nologo C:\\Windows\\system32\\slmgr.vbs /ato");
	if (s1.find("successfully") != std::string::npos)
    	std::cout << "Key installed successfully" << endl;
    else
    	std::cout << "Key didn't install successfully" << endl;
	if (s.find("successfully") != std::string::npos){
    	std::cout << "Product activated successfully" << endl;
    	break;
    }
    else
    	std::cout << "Product didn't active successfully" << endl;
    }
    std::cin.get();
}
