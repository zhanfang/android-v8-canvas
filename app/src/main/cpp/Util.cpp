//
// Created by Zhan,Fang on 2018/10/18.
//

#include "Util.h"
#include <sstream>
#include <iostream>
#include <codecvt>

using namespace v8;
using namespace std;

u16string Util::ConvertFromUtf8ToUtf16(const string& str) {
    auto utf16String =
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>().from_bytes(str);

    return utf16String;
}