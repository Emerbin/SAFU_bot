#include "pch.h"
#include "cBot.cpp"

//#include "Reminder.cpp"
//#include "HelperFunctions.h"

int main() {
    std::wstring tokenenv = _wgetenv(L"TgToken");
    std::string token(tokenenv.begin(), tokenenv.end());
    cBot c(token);
    return 0;
}
