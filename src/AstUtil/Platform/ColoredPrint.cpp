///
/// @file      ColoredPrint.cpp
/// @brief     彩色打印功能实现
/// @author    axel
/// @date      2026-04-10
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstUtil/ColoredPrint.hpp"
#include "AstUtil/Color.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/Posix.hpp"
#include "Environment.hpp"

#ifdef _WIN32
#include <windows.h>
#endif

AST_NAMESPACE_BEGIN

void cprintf(Color color, const char* format, ...)
{
    // 检查终端是否支持颜色
    static bool supportColor = aTerminalSupportColor();
    
    // 打印内容
    va_list args;
    va_start(args, format);

    if(!supportColor)
    {
        // 终端不支持颜色，使用普通打印
        posix::vprintf(format, args);
    } 
    else 
    {
        // 获取 RGB 分量
        uint8_t r, g, b, a;
        aColorToRGBA(color, r, g, b, a);
        
        // 在 Windows 平台上使用 SetConsoleTextAttribute
        #ifdef _WIN32
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        // Windows 控制台颜色映射（简化版）
        // 这里使用一个简单的映射，将 RGB 值转换为最接近的控制台颜色
        WORD consoleColor = 0;
        if (r > 127 && g > 127 && b > 127) {
            consoleColor = FOREGROUND_INTENSITY;
        }
        if (r > 127) consoleColor |= FOREGROUND_RED;
        if (g > 127) consoleColor |= FOREGROUND_GREEN;
        if (b > 127) consoleColor |= FOREGROUND_BLUE;
        SetConsoleTextAttribute(hConsole, consoleColor);
        #else
        // 在 Unix/Linux 平台上使用 ANSI 转义序列
        printf("\033[38;2;%d;%d;%dm", r, g, b);
        #endif
        
        // 打印内容
        posix::vprintf(format, args);
        
        // 恢复默认颜色
        #ifdef _WIN32
        SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        #else
        printf("\033[0m");
        #endif
    }
    
    va_end(args);
}


AST_NAMESPACE_END