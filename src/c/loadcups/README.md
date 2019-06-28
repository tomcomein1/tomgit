/** Author: TOM
* Version: 1.0
* Create: 2019-06-28
*/
1. 一个分析银联清算文件的小工具
2. 已在AIX通过编译
3. 可以解析银联 COM、TFL、ERR文件格式、解析后全部为字符串格式
4. build.sh
5. loadcups <-e|-t|-c> <cupsfile>
6. 新增loadcp.c单文件解析，(cc -o loadcp loadcp.c)动态分配内存，无需定义头文件解释处理。