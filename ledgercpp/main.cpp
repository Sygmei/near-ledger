
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
    std::tuple<ledger::Error, std::vector<uint8_t>> ret;

    ledger::Error err = nano.open();
    std::cout << ledger::error_message(err) << std::endl;

    // ret = nano.get_public_key(account);
    // std::cout << ledger::error_message(std::get<0>(ret)) << std::endl;
    // std::cout << "Publick Key: " << std::endl;
    // for (uint8_t data_byte: std::get<1>(ret))
    //     std::cout << (int)data_byte << ",";
    // std::cout << std::endl;

    ret = nano.get_version();
    std::cout << ledger::error_message(std::get<0>(ret)) << std::endl;
    std::cout << "Version: " << std::endl;
    for (uint8_t data_byte: std::get<1>(ret))
        std::cout << (int)data_byte << ".";
    std::cout << std::endl;

    const std::vector<uint8_t> signable_tx = {
        20,0,0,0,108,101,100,103,101,114,103,97,109,101,115,50,46,116,101,115,116,110,101,116,0,191,217,62,20,113,68,168,64,193,148,49,64,151,0,208,244,69,221,23,94,60,139,209,167,237,247,57,103,18,235,172,11,199,208,85,142,133,85,0,0,9,0,0,0,116,101,115,116,46,110,101,97,114,43,38,82,152,20,83,70,135,150,46,91,191,61,18,60,128,77,231,72,20,150,248,160,195,84,158,101,170,11,109,22,72,1,0,0,0,3,0,0,0,161,237,204,206,27,194,211,0,0,0,0,0,0
    };
    ret = nano.sign(account, signable_tx, /*p1==P1_LAST*/0x80);
    std::cout << ledger::error_message(std::get<0>(ret)) << std::endl;
        std::cout << "Signature: " << std::endl;
    for (uint8_t data_byte: std::get<1>(ret))
        std::cout << (int)data_byte << ",";
    std::cout << std::endl;

    nano.close();
}
