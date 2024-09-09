#include "TextUI.h"
#include <iostream>

int main() {
    TextUI ui;
    
    ui.processCommand();  // 根據返回值決定是否繼續運行

    return 0;
}
