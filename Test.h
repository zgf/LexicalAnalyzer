#pragma once
#include "RegexDFA.h"
/*要支持取反字符集*/ //FINISH
/*DFA状态最小化*/ //Finish
/*错误处理 和调试信息支持*/
/*工程结构分析和注释完善*/
/*一些丑陋的函数和模块重构 学习下重构工具的使用*/
/*要做cpp 和h文件分离*/ //Finish 换了种方式解决了visitor模式问题
/*创建闭包哪里效率太低了- -*/ //已经改进了30%效率...也许用hash代替vector更快?0 0 使用Release模式更快不过那个goto函数哪里还是要继续优化
/*Template中的耦合部分要改进*/
/*NFA部分写成扩展正则部分的那种*/

/*正则引擎应该能DFA的地方就DFA不能DFA地方再NFA*/

/*去完善NFA部分*/
/*visitor模式批量生成工具*/

