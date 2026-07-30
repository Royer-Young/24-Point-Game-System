#pragma once
#include <string>
#include <cctype>
#include<cmath>
using namespace std;

class Expression {
private:
    string expr;  // 存储当前表达式，比如 "(3+5)*7-9"

public:
    Expression() {}

    // ---------- 对外接口：Game类会调用这些 ----------

    void appendNumber(int number) {
        expr = expr + to_string(number);
    }      // 追加一个完整数字，比如11

    void appendOperator(char op) {
        expr = expr + op;
    }       // 追加一个符号: + - * / ( )

    void clear() {
        expr.clear();
    }// 清空表达式

    string getExpressionString() const {
        return expr;
    } // 拿到表达式字符串，给界面显示

    void setExpressionString(const string& s) {
        expr = s;
    }

    bool evaluate(double& result) const {
        if (expr.empty()) return false;

        size_t pos = 0;
        double val;
        if (!parseExpr(pos, val)) return false;
        if (pos != expr.size()) return false;
            result = val;
            return true;
        //把val写进result，return true
    }

    bool isEqual24() const {
        double result;
        if (!evaluate(result)) return false;
        if (fabs(result - 24.0) <= 1e-6) return true;
        return false;
        // TODO: 调用evaluate，失败的话（比如表达式不合法）直接return false
        // 成功的话，判断 fabs(result - 24.0) 是否小于一个很小的数，比如 1e-6
    }// 判断结果是否等于24

    void removeChars(int n) {
        if (n > (int)expr.size()) n = (int)expr.size();  // 防止删多了越界
        expr.resize(expr.size() - n);   // 直接截断字符串末尾n个字符
    }

private:
    // ---------- 内部实现：递归下降解析器 ----------
    // 甲：管加减，调用乙
    bool parseExpr(size_t& pos, double& result) const {
        if (!parseTerm(pos, result)) return false;  // 先拿第一个数

        while (true) {
            if (pos >= expr.size()) break;

            char op = expr[pos];
            if (op != '+' && op != '-') break;

            pos++;

            double rhs;
            if (!parseTerm(pos, rhs)) return false;

            if (op == '+'){
                result = result + rhs;
            }
            else result = result - rhs;
            // TODO: 参考parseTerm的写法，判断是否越界、判断是否是+或-
            // 如果是+或-，跳过符号，调用parseTerm拿到rhs，做加/减，写回result
            // 否则break
        }

        return true;
    }

    // 乙：管乘除，调用丙
    bool parseTerm(size_t& pos, double& result) const {
        if (!parseFactor(pos, result)) return false;  // 先拿第一个数

        while (true) {
            if (pos >= expr.size()) break;  // 读完了，停止循环

            char op = expr[pos];
            if (op != '*' && op != '/') break;  // 不是乘除，停止循环

            pos++;  // 跳过这个符号

            double rhs; // TODO: 调用parseFactor拿到符号右边的数，写进rhs，失败要return false
            if (!parseFactor(pos, rhs)) return false;

            if (op == '*') {
                result=result*rhs;
                // TODO: result 乘上 rhs
            }
            else {
                if (rhs == 0.0) {
                    return false;
                }
                else result = result / rhs;
                // TODO: result 除以 rhs（提示：如果rhs是0，这是非法的，要return false）
            }
        }

        return true;
    }

    // 丙：管纯数字 + 括号（遇到括号会反过来调用甲）
    bool parseFactor(size_t& pos, double& result) const {
        if (pos >= expr.size()) return false;  // 越界，没东西可读

        if (expr[pos] == '(') {
            pos++;  // 跳过 '('

            if (!parseExpr(pos, result)) return false;  // 递归调用甲，算括号里的值

            if (pos >= expr.size() || expr[pos] != ')') {
                return false;  // 没有对应的右括号，括号不匹配
            }
            pos++;  // 跳过 ')'

            return true;
        }

        // 走到这里说明不是括号，应该是纯数字
        if (!isdigit(static_cast<unsigned char>(expr[pos]))) {
            return false;
        }
        size_t start = pos;
        while (pos < expr.size() && isdigit(static_cast<unsigned char>(expr[pos]))) {
            pos++;
        }
        result = stod(expr.substr(start, pos - start));
        return true;
    }
};
