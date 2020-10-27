#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QTextCodec>
#include <fstream>
#include <QDebug>

const int numOfKeyword = 61;    //关键字数量
const int numOfMO = 16;     //单目运算符数量
const int numOfBO = 20;     //双目运算符数量
const int numOfD = 11;       //界符数量
//关键字
const char keyword[61][17] = {"auto", "enum", "operator", "throw",
                              "bool", "explicit", "private", "true",
                              "break", "export", "protected", "try",
                              "case", "extern", "public", "typedef",
                              "catch", "false", "register", "typeid",
                              "char", "float", "reinterpret_cast", "typename",
                              "class", "for", "return", "union",
                              "const", "friend", "short", "unsigned",
                              "const_cast", "goto", "signed", "using",
                              "continue", "if", "sizeof", "virtual",
                              "default", "inline", "static", "void",
                              "delete", "int", "static_cast", "volatile",
                              "do", "long", "struct", "wchar_t",
                              "double", "mutable", "switch", "while",
                              "dynamic_cast", "namespace", "template", "include", "main"};
//单目运算符
const char monocularOperator[16] = {'+', '-', '*', '/', '%', '>', '<', '!', '&', '|', '^', '~', '=', ',', '.', ':'};
//双目运算符
const char binocularOperator[20][4] = {"++", "--", "==", "!=", ">=", "<=", "&&", "||", "<<", ">>",
                                       "+=", "-=", "*=", "/=", "%=", "&=", "^=", "|=", "->", "::"};
//界符
const char delimiter[11] = {',', '(', ')', '{', '}', ';', '<', '>', '#', '[', ']'};

FILE *fileSource = nullptr;

bool isDigital(char a) {
    if (a >= '0' && a <= '9')
        return true;
    return false;
}

bool isLetter(char a) {
    if (a >= 'a' && a <= 'z')
        return true;
    if (a >= 'A' && a <= 'Z')
        return true;
    return false;
}

bool isKeyword(char a[]) {
    int len = strlen(a);
    for (int i = 0; i < numOfKeyword; ++i) {
        if (strlen(keyword[i]) == len) {
            if (strcmp(keyword[i], a) == 0)
                return true;
        }
    }
    return false;
}

bool isMO(char a) {
    for (int i = 0; i < numOfMO; ++i) {
        if (monocularOperator[i] == a)
            return true;
    }
    return false;
}

bool isBO(char a[]) {
    for (int i = 0; i < numOfBO; ++i) {
        if (strcmp(binocularOperator[i], a) == 0)
            return true;
    }
    return false;
}

bool isDelimiter(char a) {
    for (int i = 0; i < numOfD; ++i) {
        if (delimiter[i] == a)
            return true;
    }
    return false;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_browseButton_clicked() {
    QString dirPath;
    dirPath = QFileDialog::getOpenFileName(this, tr("文件"), "./text/", tr("text(*.cpp *.c *.h *.txt)"));
    ui->dirPathText->setText(dirPath);
}

void MainWindow::on_confirmButton_clicked() {
    ui->browserText->clear();

    QString dirPath;
    dirPath = ui->dirPathText->toPlainText();

    QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
    std::string sDirPath = code->fromUnicode(dirPath).data();
    const char *cDirPath = sDirPath.c_str();

    fileSource = fopen(cDirPath, "r");

    if (!fileSource) {
        ui->browserText->setText("文件不存在！！！");
    } else {
        char ch;
        char str[10000];
        int point;
        int flag;

        ch = fgetc(fileSource);
        bool finish = false;
        bool letter = false;
        int dot = 0; //记录小数点的个数
        bool delimiterFlag = false; // <>的分界符标记

        while (!finish) {
            flag = -1;  //无法识别的符号即为-1
            point = 0;

            //判断是否为数字
            if (isDigital(ch)) {
                flag = 1;
                str[point++] = ch;
                ch = fgetc(fileSource);
                while (isLetter(ch) || isDigital(ch) || ch == '.') {
                    if (isLetter(ch))
                        letter = true;
                    if (ch == '.')
                        dot++;
                    flag = 1;
                    str[point++] = ch;
                    ch = fgetc(fileSource);
                }
                str[point] = '\0';
            }
            if (flag == 1) {
                if (letter) {
                    point = 0;
                    flag = -1;
                    letter = false;
                    dot = 0;
                } else if (dot > 1) {
                    point = 0;
                    flag = -1;
                    letter = false;
                    dot = 0;
                } else {
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t常量";
                    ui->browserText->append(qStr);
                    qStr = "";
                    point = 0;
                    flag = -1;
                    letter = false;
                    dot = 0;
                }
            }
            //判断是否为单词
            if (isLetter(ch)) {
                flag = 2;
                str[point++] = ch;
                ch = fgetc(fileSource);
                while (isLetter(ch) || isDigital(ch) || ch == '_') {
                    flag = 2;
                    str[point++] = ch;
                    ch = fgetc(fileSource);
                }
                str[point] = '\0';
            }
            if (flag == 2) {
                if (isKeyword(str)) {
                    if (strcmp(str, "include") == 0)
                        delimiterFlag = true;
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t关键字";
                    ui->browserText->append(qStr);
                    qStr = "";
                } else {
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t标识符";
                    ui->browserText->append(qStr);
                    qStr = "";
                }
                point = 0;
                flag = -1;
            }
            //判断字符或字符串
            if (ch == '\"' || ch == '\'') {
                flag = 3;
                str[point++] = ch;
                if (ch == '\'') {
                    while (true) {
                        ch = fgetc(fileSource);
                        str[point++] = ch;
                        if ((ch == '\'' && str[point - 2] != '\\') || (ch == '\'' && str[point - 2] == '\\' && str[point - 3] == '\\'))
                            break;
                    }
                    str[point] = '\0';
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t字符";
                    ui->browserText->append(qStr);
                    qStr = "";
                } else {
                    while (true) {
                        ch = fgetc(fileSource);
                        str[point++] = ch;
                        if ((ch == '\"' && str[point - 2] != '\\') || (ch == '\"' && str[point - 2] == '\\' && str[point - 3] == '\\'))
                            break;
                    }
                    str[point] = '\0';
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t字符串";
                    ui->browserText->append(qStr);
                    qStr = "";
                }
                ch = fgetc(fileSource);
                point = 0;
            }
            //判断是否为运算符
            if (isMO(ch)) {
                flag = 4;
                str[point++] = ch;
                if ((ch = fgetc(fileSource)) == EOF) {
                    finish = true;
                }
                str[point++] = ch;
                if (str[0] == '/' && str[1] == '/') {   //判断是否为单行注解
                    while ((ch = fgetc(fileSource)) != '\n') {
                        if (ch == EOF) {
                            finish = true;
                            break;
                        }
                        str[point++] = ch;
                    }
                    str[point] = '\0';
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t单行注解";
                    ui->browserText->append(qStr);
                    qStr = "";
                    if (finish)
                        break;
                    ch = fgetc(fileSource);
                    if (ch == EOF)
                        finish = true;
                    continue;
                }
                if (str[0] == '/' && str[1] == '*') {  //判断是否为多行注解
                    while (str[point-2] != '*' || str[point-1] != '/') {
                        ch = fgetc(fileSource);
                        str[point++] = ch;
                    }
                    str[point] = '\0';
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t多行注解";
                    ui->browserText->append(qStr);
                    qStr = "";
                    ch = fgetc(fileSource);
                    continue;
                }
                str[point] = '\0';
                if (finish == false && isBO(str)) {
                    QString qStr(str);
                    qStr = qStr + "\t\t\t\t双目运算符";
                    ui->browserText->append(qStr);
                    qStr = "";
                    ch = fgetc(fileSource);
                } else {
                    if ((str[0] == '<' || str[0] == '>') && delimiterFlag) {
                        str[point] = '\0';
                        QString qStr(str[0]);
                        qStr = qStr + "\t\t\t\t分界符";
                        ui->browserText->append(qStr);
                        qStr = "";
                        if (str[0] == '>')
                            delimiterFlag = false;
                    } else {
                        str[point] = '\0';
                        QString qStr(str[0]);
                        qStr = qStr + "\t\t\t\t单目运算符";
                        ui->browserText->append(qStr);
                        qStr = "";
                    }

                }
                point = 0;
            }
            //判断是否为特殊符号
            if (isDelimiter(ch)) {
                flag = 5;
                QString qStr(ch);
                qStr = qStr + "\t\t\t\t分界符";
                ui->browserText->append(qStr);
                qStr = "";
                if ((ch = fgetc(fileSource)) == EOF) {
                    finish = true;
                    break;
                }
            }
            if (ch == ' ' || ch == '\n' || ch == '\t') {
                flag = 6;
                if ((ch = fgetc(fileSource)) == EOF) {
                    finish = true;
                    break;
                }
                continue;
            }
            if (flag == -1) {   //其他符号
                if (ch == EOF)
                    finish = true;
                else
                    ch = fgetc(fileSource);
            }
        }
    }
}

void MainWindow::on_compressButton_clicked() {

    ui->browserText->clear();

    QString dirPath;
    dirPath = ui->dirPathText->toPlainText();

    //获取文件名
    QFileInfo fileInfo(dirPath);
    QString fileName = fileInfo.fileName();
    QStringList list = fileName.split('.');

    //获取文件的存放位置
    QString filePath = fileInfo.path();

    QString newFileName = "./compress/" + list[0] + "_compress." + list[1];

    QTextCodec *code = QTextCodec::codecForName("GB2312");//解决中文路径问题
    std::string sDirPath = code->fromUnicode(dirPath).data();
    const char *cDirPath = sDirPath.c_str();

    std::string sNewDirPath = code->fromUnicode(newFileName).data();
    const char *cNewDirPath = sNewDirPath.c_str();

    std::ifstream input(cDirPath, std::ios::in);
    std::ofstream output(cNewDirPath, std::ios::out);

    if (input && output) {
        int flag = -1;
        bool keywordFlag = false;
        char ch;
        char str[100000];
        int point = 0;
        ch = input.get();
        while (ch != EOF) {
            if (isLetter(ch)) { //关键字后面保留一个空格
                flag = 1;
                str[point++] = ch;
                ch = input.get();
                while (isLetter(ch) || isDigital(ch) || ch == '_') {
                    flag = 1;
                    str[point++] = ch;
                    ch = input.get();

                }
                str[point] = '\0';
            }
            if (flag == 1) {
                if (isKeyword(str)) {
                    keywordFlag = true;
                    if (strcmp(str, "include") == 0)
                        keywordFlag = false;
                    output<<str;
                } else {
                    output<<str;
                }
                point = 0;
                flag = -1;
            }
            //判断是否为运算符
            if (isMO(ch)) {
                flag = 2;
                str[point++] = ch;
                ch = input.get();
                str[point++] = ch;
                if (str[0] == '/' && str[1] == '/') {   //判断是否为单行注解
                    while ((ch = input.get()) != '\n') {
                        str[point++] = ch;
                        if (ch == EOF)
                            break;
                    }
                    str[point] = '\0';
                    ch = input.get();
                    point = 0;
                    continue;
                }
                if (str[0] == '/' && str[1] == '*') {  //判断是否为多行注解
                    while (str[point-2] != '*' || str[point-1] != '/') {
                        ch = input.get();
                        str[point++] = ch;
                    }
                    str[point] = '\0';
                    ch = input.get();
                    if (ch == EOF)
                        break;
                    point = 0;
                    continue;
                }
                str[point] = '\0';
                if (isBO(str)) {
                    output<<str;
                    ch = input.get();
                } else {
                    str[point] = '\0';
                    output<<str[0];
                }
                point = 0;
            }
            if (ch == ' ') {
                flag = 3;
                if (keywordFlag) {
                    output<<ch;
                    keywordFlag = false;
                    ch = input.get();
                } else {
                    ch = input.get();
                }
            }
            if (ch == '\n' || ch == '\t') {
                ch = input.get();
            }
            if (!isLetter(ch) && !isMO(ch) && ch != ' ' && ch != '\t' && ch != '\n') {

                output<<ch;
                ch = input.get();
                if (ch == EOF)
                    break;
            }
        }
        QString qStr = "文件保存为" + newFileName;
        ui->browserText->append("压缩成功！！！");
        ui->browserText->append(qStr);
        input.close();
        output.close();
    } else {
        ui->browserText->setText("文件不存在！！！");
    }


}
