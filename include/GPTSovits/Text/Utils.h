//
// Created by 19254 on 24-11-30.
//

#ifndef GPT_SOVITS_CPP_UTILS_H
#define GPT_SOVITS_CPP_UTILS_H

#include <string>
#include <vector>

namespace GPTSovits::Text {

/**
 * @brief 根据正则表达式分割输入字符串
 *
 * 此函数根据提供的正则表达式分割输入字符串，并将结果存储在结果向量中。
 *
 * @param[out] result 存储分割结果的向量
 * @param[in] input 需要分割的输入字符串
 * @param[in] delimiterPattern 用于分割的正则表达式模式
 *
 * @note 此函数会忽略空的分割部分，并在遇到分隔符时将其添加到当前部分。
 *       最后，函数会将最后一部分添加到结果中。
 *
 * @example
 * @code
 * std::vector<std::string> result;
 * std::string input = "abc,def,ghi";
 * std::string delimiter = ",";
 * SplitByRegex(result, input, delimiter);
 * // result 现在包含 {"abc,", "def,", "ghi"}
 * @endcode
 */
void SplitByRegex(std::vector<std::string> &result, const std::string &source, const std::string &regex=R"([,.;?!、，。？！;：…])");
/**
 * @brief 根据正则表达式分割输入字符串
 *
 * 此函数根据提供的正则表达式分割输入字符串，并将结果存储在结果向量中。
 *
 * @param[out] result 存储分割结果的向量
 * @param[in] input 需要分割的输入字符串
 * @param[in] delimiterPattern 用于分割的正则表达式模式
 *
 * @note 此函数会忽略空的分割部分，并在遇到分隔符时将其添加到当前部分。
 *       最后，函数会将最后一部分添加到结果中。
 *
 * @example
 * @code
 * std::vector<std::u32string> result;
 * std::u32string input = U"abc,def,ghi";
 * std::u32string delimiter = U",";
 * SplitByRegex(result, input, delimiter);
 * // result 现在包含 {U"abc,", U"def,", U"ghi"}
 * @endcode
 */
void SplitByRegex(std::vector<std::u32string> &result, const std::u32string &source, const std::u32string &regex);

void SplitByRegex1(std::vector<std::u32string>&result,const std::u32string& text,const std::u32string&marks);
void FindAll1(std::vector<std::u32string>&result,const std::u32string& text,const std::u32string&marks);
void tolower(std::u32string& str);

/**
 * @brief 检查给定的源字符串是否以指定的字符结尾。
 *
 * 此函数接受一个源字符串和一个字符集合，判断源字符串的最后一个字符是否
 * 存在于字符集合中。如果源字符串为空，则返回 false。
 *
 * @param source 要检查的源字符串，类型为 std::string。
 * @param checks 要检查的字符集合，类型为 std::vector<char>。
 * @return 如果源字符串的最后一个字符在字符集合中，则返回 true；否则返回 false。
 *
 * @exception 无
 *
 * @example
 * @code
 * std::string str = "Hello, World!";
 * std::vector<char> checks = {'!', '?'};
 * bool result = FindEnd(str, checks); // result 为 true，因为字符串以 '!' 结尾。
 * @endcode
 */
bool FindEnd(const std::string &source, const std::vector<char> &checks);
bool FindEnd(const std::string &source, const std::vector<std::string> &checks);
/**
 * @brief 检查给定的源字符串是否以指定的字符结尾。
 *
 * 此函数接受一个源字符串和一个字符集合，判断源字符串的最后一个字符是否
 * 存在于字符集合中。如果源字符串为空，则返回 false。
 *
 * @param source 要检查的源字符串，类型为 std::u32string。
 * @param checks 要检查的字符集合，类型为 std::vector<char32_t>。
 * @return 如果源字符串的最后一个字符在字符集合中，则返回 true；否则返回 false。
 *
 * @exception 无
 *
 * @example
 * @code
 * std::u32string str = U"Hello, World!";
 * std::vector<char32_t> checks = {U'!', U'?'};
 * bool result = FindEnd(str, checks); // result 为 true，因为字符串以 '!' 结尾。
 * @endcode
 */
bool FindEnd(const std::u32string &source, const std::vector<char32_t> &checks);
bool FindEnd(const std::u32string &source, const std::vector<std::u32string> &checks);

/**
 * @brief 检查给定的字符是否为字母字符。
 *
 * 该函数确保输入的字符在有效范围内（0到255），
 * 然后使用标准库函数检查该字符是否为字母。
 *
 * @param c 要检查的字符（整数类型）。
 * @return 如果字符是字母字符，返回 true；否则返回 false。
 */
bool safe_isalpha(int c);

/**
 * @brief 检查给定的字符是否为数字字符。
 *
 * 该函数确保输入的字符在有效范围内（0到255），
 * 然后使用标准库函数检查该字符是否为数字。
 *
 * @param c 要检查的字符（整数类型）。
 * @return 如果字符是数字字符，返回 true；否则返回 false。
 */
bool safe_isdigit(int c);

/**
 * @brief 检查给定的字符是否为空格字符。
 *
 * 该函数检查输入的字符是否为常见的空白字符，包括空格、制表符、
 * 换行符、回车符、换页符和垂直制表符。
 *
 * @param ch 要检查的字符（Unicode字符类型）。
 * @return 如果字符是空白字符，返回 true；否则返回 false。
 */
bool safe_isspace(char32_t ch);

bool ispunct(char32_t ch);

/**
 * @brief 修剪字符串两端的空格字符。
 *
 * 此函数接受一个 UTF-32 编码的字符串，并返回去除前后空格后的子字符串。
 *
 * @param str 要修剪的输入字符串。
 * @return std::u32string 返回去除前后空格后的字符串。
 *
 * @exception 无。
 *
 * @example
 * @code
 * std::u32string original = U" \t  Hello, World! \n  ";
 * std::u32string trimmed = U32trim(original);
 * // trimmed 现在是 U"Hello, World!"
 * @endcode
 */
std::u32string U32trim(const std::u32string &str);

/**
 * @brief 获取给定字符串中 UTF-8 字符的边界索引。
 *
 * 此函数解析输入的 UTF-8 编码字符串，并返回每个字符的起始索引和
 * 字符的字节长度，以便于后续处理。
 *
 * @param input 输入的 UTF-8 编码字符串。
 * @return 返回一个包含每个 UTF-8 字符的边界索引的向量，每个元素为一对
 *         整数，第一个整数为字符的起始索引，第二个整数为该字符的字节长度。
 *
 *
 * @example
 * @code
 * std::string text = "你好，世界！";
 * auto indices = UTF8CharacterBoundaryIndices(text);
 * for (const auto &index : indices) {
 *     std::cout << "起始索引: " << index.first << ", 字节长度: " << index.second << std::endl;
 * }
 * @endcode
 */
std::vector<std::pair<int, int>> UTF8CharacterBoundaryIndices(const std::string &input);

};

#endif //GPT_SOVITS_CPP_UTILS_H
