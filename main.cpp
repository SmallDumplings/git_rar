#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <cstdint>
#include <windows.h>

#pragma pack(push, 1)
using namespace std;

struct rar_header{
    uint16_t header_crc;
    uint8_t header_type;
    uint16_t header_flags;
    uint16_t header_size;
};

struct rar_header_atribut{
    uint32_t PackSize;
    uint32_t UnpSize;
    uint8_t HostOS;
    uint32_t FileCRC;
    uint32_t FileTime;
    uint8_t UnpVer;
    uint8_t Method;
    uint16_t NameSize;
    uint32_t FileAttr;
};

#pragma pack(pop)
void read_content(vector<char> &rar_data, int fileSize){
    int offset = 7;
    rar_header* r_header = reinterpret_cast<rar_header*>(&rar_data[offset]);
    offset += int(r_header -> header_size);

    while (true){
        rar_header* r_header = reinterpret_cast<rar_header*>(&rar_data[offset]);
        rar_header_atribut* r_atribut = reinterpret_cast<rar_header_atribut*>(&rar_data[offset+7]);

        if (r_header->header_type  == 0x74){
            uint16_t sizeName = r_atribut -> NameSize;
            uint16_t offsetHeader = offset + sizeof(rar_header) + sizeof(rar_header_atribut);

            if (offsetHeader +sizeName > rar_data.size()) break;

            char* headerData = reinterpret_cast<char*>(&rar_data[offsetHeader]);
            string utf8line(headerData, sizeName);
            vector <wchar_t> utf16content(sizeName);
            MultiByteToWideChar(CP_UTF8,0,utf8line.c_str(),sizeName,utf16content.data(),utf16content.size());
            wstring FileName(utf16content.begin(),utf16content.end());
            wcout << "File name: " << FileName << endl;

            offset += r_header -> header_size + r_atribut->PackSize;
        }
        else{
            offset += r_header->header_size;
        }



    }
}

int main()
{
   setlocale(LC_ALL, "rus");
    // Открыть файл в двоичном режиме
    ifstream rar_file("Example.rar", ios::binary);

    // Убедиться, что открылся
    if(rar_file.is_open())
    {
        // Определить размер файла
        rar_file.seekg(0, ios::end);
        int fileSize = rar_file.tellg();
        cout << "File size: " << fileSize << endl;
        cout << endl;
        rar_file.seekg(0, ios::beg);


        // Выделить память и считать данные
        vector<char> rar_data(fileSize, 0);
        rar_file.read(rar_data.data(), fileSize);

        read_content(rar_data, fileSize);

    }

    return 0;
}
