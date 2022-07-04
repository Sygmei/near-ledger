
#include <iostream>
#include <ledgercpp/ledger.hpp>

#include <algorithm>
#include <string>

std::vector<std::string> split(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> tokens;
    std::string::size_type last_pos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos = str.find_first_of(delimiters, last_pos);
    while (std::string::npos != pos || std::string::npos != last_pos)
    {
        tokens.push_back(str.substr(last_pos, pos - last_pos));
        last_pos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, last_pos);
    }
    return tokens;
}

bool ends_with(const std::string& string, const std::string& search)
{
    if (string.size() < search.size())
    {
        return false;
    }
    return (
        std::mismatch(search.rbegin(), search.rend(), string.rbegin()).first == search.rend());
}

bool is_string_int(const std::string& str)
{
    if (!str.empty())
    {
        return std::all_of(str.begin(), str.end(), isdigit);
    }
    return false;
}

std::vector<uint8_t> bip32_path_to_bytes(std::string bip32_path, bool prepend_length = false) {
    constexpr uint32_t HARDENED_FLAG = 0x80000000;

    std::transform(bip32_path.begin(), bip32_path.end(), bip32_path.begin(),
        [](unsigned char c) { return std::tolower(c); });
    if (bip32_path.substr(0, 2) == "m/")
    {
        bip32_path = bip32_path.substr(2);
    }

    const std::vector<std::string> path_parts = split(bip32_path, "/");
    std::vector<uint8_t> bytes;
    bytes.reserve(path_parts.size() * 4 + ((prepend_length) ? 1 : 0));
    if (prepend_length)
    {
        bytes.push_back(path_parts.size());
    }
    for (std::string part : path_parts)
    {
        bool hardened = false;
        if (ends_with(part, "'"))
        {
            hardened = true;
            part = part.substr(0, part.size() - 1);
        }
        if (!is_string_int(part))
        {
            throw std::runtime_error("invalid bip32 path");
        }
        uint32_t part_as_uint32 = std::stoul(part);
        if (hardened)
        {
            part_as_uint32 += HARDENED_FLAG;
        }
        bytes.push_back(static_cast<uint8_t>(part_as_uint32 >> 24));
        bytes.push_back(static_cast<uint8_t>(part_as_uint32 >> 16));
        bytes.push_back(static_cast<uint8_t>(part_as_uint32 >> 8));
        bytes.push_back(static_cast<uint8_t>(part_as_uint32));
    }
    return bytes;
}

int main(int argc, char** argv)
{
    const std::vector<uint8_t> account = bip32_path_to_bytes("44'/397'/0'/0'/1'");
    ledger::Ledger nano;
    auto err = nano.open();
    std::cout << ledger::error_message(err) << std::endl;
    // auto err2 = nano.get_public_key(account, false);
    // std::cout << ledger::error_message(std::get<0>(err2)) << std::endl;
    const std::vector<uint8_t> tx_data = { 24, 0, 0, 0, 116, 101, 115, 116, 45, 99, 111, 110, 110, 101, 99, 116, 45, 108, 101, 100, 103, 101, 114, 46, 116, 101, 115, 116, 0, 114, 6, 142, 2, 154, 152, 9, 199, 242, 218, 121, 200, 162, 116, 116, 63, 245, 247, 237, 206, 36, 214, 72, 189, 19, 233, 127, 112, 147, 160, 49, 50, 4, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 118, 103, 80, 220, 110, 71, 221, 204, 178, 66, 31, 39, 222, 194, 6, 213, 234, 70, 148, 66, 42, 28, 47, 195, 92, 168, 194, 227, 99, 138, 2, 170, 207, 75, 1, 0, 0, 0, 3, 0, 0, 64, 189, 139, 91, 147, 107, 108, 0, 0, 0, 0, 0, 0, 0 };
    const std::vector<uint8_t> actually_signable_tx_data = { 128,0,0,44,128,0,1,141,128,0,0,0,128,0,0,0,128,0,0,1,64,0,0,0,51,54,49,49,100,53,52,56,97,55,54,52,51,56,101,50,52,56,56,50,99,54,51,48,49,52,54,49,101,97,51,48,98,53,97,100,57,97,97,99,102,99,102,100,55,100,55,100,97,50,53,53,99,48,52,49,49,53,102,50,102,101,53,98,0,54,17,213,72,167,100,56,226,72,130,198,48,20,97,234,48,181,173,154,172,252,253,125,125,162,85,192,65,21,242,254,91,199,121,220,40,2,55,0,0,9,0,0,0,97,117,98,101,46,110,101,97,114,8,246,144,251,125,85,26,99,17,74,127,4,26,202,72,2,76,151,68,138,232,23,199,231,124,51,125,23,63,53,131,87,1,0,0,0,3,0,0,64,123,165,240,99,129,150,10,0,0,0,0,0,0 };
    auto err3 = nano.sign(account, tx_data);
    std::cout << ledger::error_message(std::get<0>(err3)) << std::endl;
    nano.close();
}