//
// Created by Alone on 2022-1-24.
//
//TODO aaaaaaa得出感悟：1.数据较为复杂的情况下尽量不要使用全局变量 2.在构造函数初始化的时候千万不要直接new空间给它，记得随时随地nullptr
#include <fstream>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <windows.h>
#include <vector>
#include <unordered_map>
#include <ctime>
#include <filesystem>
#include <random>

#define IMGS_PATH "./initImg.txt"
#define MOB_PATH         "./mob.txt"
#define CATEGORIES_PATH "./categories.txt"
#define BLOG_SRC "./BlogPath.txt"
std::filesystem::path POSTS_PATH;//用于获取post_path

using namespace std;
//TODO 建立枚举映射
enum class SHOW_ARGS : int {
    EMPTY, CATEGORIES, IMG ,BLOG_PATH
};
//TODO 命令行参数的枚举映射
unordered_map<string, SHOW_ARGS> MAP{
        {"-sc", SHOW_ARGS::CATEGORIES},
        {"-si", SHOW_ARGS::IMG},
        {"-sp", SHOW_ARGS::BLOG_PATH}
};

//TODO 封装文件读取类
class FileReader {
    stringstream in_buf;
    ifstream reader;
public:
    FileReader() = default;

    FileReader(const FileReader &) = delete;

    FileReader(FileReader &&) = delete;

    ~FileReader() {
        if(reader.is_open())
            reader.close();
    }

    void open(const string &path) {
        reader.open(path);
        if (!reader.is_open()) {
            perror("reader open failed");
            exit(1);
        }
        in_buf << reader.rdbuf();
    }

    bool readAll(string &dst) {
        if (in_buf.good())
            dst = in_buf.str();
        else
            return false;
        return true;
    }

    bool readline(string &dst) {
        if (in_buf.good())
            getline(in_buf, dst);
        else return false;
        return true;
    }
};

//TODO 封装文件写入类
class FileWriter {
    char *out_buf;
    ofstream writer;
    size_t cur_buf_size;
    size_t max_buf_size;
private:
    void _write() {  //缓冲区写满，写入文件中
        writer.write(out_buf, max_buf_size);
        cur_buf_size = 0;
    }

public:
    FileWriter() : cur_buf_size(0), max_buf_size(512), out_buf(nullptr) {};

    FileWriter(const FileReader &) = delete;

    FileWriter(FileReader &&) = delete;

    FileWriter(const string &path, ios::openmode mode = ios::out) {
        writer.open(path, mode);
        if (!writer.is_open()) {
            perror("writer open failed");
            exit(1);
        }
        cur_buf_size = 0;
        max_buf_size = 512;
        out_buf = new char[max_buf_size + 5];
    }

    ~FileWriter() {
        if (cur_buf_size > 0) {
            writer.write(out_buf, cur_buf_size);
            cur_buf_size = 0;
        }
        delete[] out_buf;
        out_buf = nullptr;
        writer.flush();
        writer.close();
    }

    static bool exist(const string &path) {
        return (access(path.c_str(), F_OK) != -1);
    }

    void open(const string &path, ios::openmode mode = ios::out) {
        writer.open(path, mode);
        if (!writer.is_open()) {
            perror("writer open failed");
            exit(1);
        }
        out_buf = new char[max_buf_size + 5];
    }

    void write(const string &src) {//TODO 缓冲机制的重要组成
        if (writer.is_open()) {//只有在open文件后才能写入
            if (src.empty()) return;
            if (cur_buf_size == max_buf_size)
                _write();
            size_t psize = src.size() + cur_buf_size;//如果全盘写入缓冲区后，缓冲区需要的大小
            int startp = 0, maxLen;
            while (psize > max_buf_size) {  //当这次写入缓冲区的数据量大于缓冲区的大小，则进行不断写满更新操作
                maxLen = max_buf_size - cur_buf_size;
                copy(src.begin() + startp, src.begin() + startp + maxLen, out_buf + cur_buf_size);//copy到满状态，再来一次write
                _write();
                startp += maxLen;
                psize -= max_buf_size;
            }
            //如果写入数据不超出缓冲区大小，则直接写入
            copy(src.begin() + startp, src.end(), out_buf + cur_buf_size);
            cur_buf_size += src.size() - startp;
        }
    }

    FileWriter &append(const string &src) {//TODO 和write没区别，只是支持链式调用
        write(src);
        return *this;
    }
};


//TODO 整个项目需要操作的变量（很不推荐用全局变量，我就是因为这玩意就导致了bug）
FileReader readImg, readText, readCategories;//用于文件io的变量
FileWriter appendCategories, fileWriter;
vector<string> imgs, categories;         //用于存下磁盘到内存的数据，根据名字判断存的啥
time_t now = time(NULL);


//TODO 打开typora软件
void open_exe_from_path(const char *path) {
    WinExec(path, SW_SHOWNORMAL);
    cout << "open your custom editor successfully!" << endl;
}

//TODO 打印内容
void show(vector<string> &src) {
    for (int i = 0; i < src.size(); i++) {
        if (!src[i].empty())
            printf("%d: %s\n", i, src[i].c_str());
    }
}

//TODO 替换string的内容
void to_replace(string &s, const string &target, const string &replacement) {
    int i = 0;
    int find_ret;
    int tar_len = target.size();
    while ((find_ret = s.find(target, i)) != -1) {
        s.replace(find_ret, tar_len, replacement);
        i = find_ret;
    }
}

//TODO 打印出错的信息，并退出程序
void exit_print(const char *content) {
    printf("running failed: %s\n", content);
    exit(1);
}

//TODO 得到当前的时间
string get_cur_time() {
    tm *tm_t = localtime(&now);
    char c_time[50];
    sprintf(c_time, "%04d-%02d-%02d",
            tm_t->tm_year + 1900,
            tm_t->tm_mon + 1,
            tm_t->tm_mday);
    return string(c_time);
}


//TODO 基本的初始化数据
void InitInputFileInfo() {
    readImg.open(IMGS_PATH);
    readText.open(MOB_PATH);
    readCategories.open(CATEGORIES_PATH);
}

void InitOutputFileInfo(const string &targetPath) {
    fileWriter.open(targetPath);
    appendCategories.open(CATEGORIES_PATH, ios::app);
}

void InitImgs() {
    string tmp;
    while (readImg.readline(tmp)) {
        if(!tmp.empty())
            imgs.push_back(tmp);
    }
    //todo 经过洗牌算法把数组里面的内容打乱
    shuffle(imgs.begin(),imgs.end(), std::default_random_engine(now));
}

void InitCategories() {
    string tmp;
    while (readCategories.readline(tmp)) {
        categories.push_back(tmp);
    }
}
void InitPostPath(){
    FileReader fd;
    fd.open(BLOG_SRC);
    string pth;
    fd.readline(pth);
    POSTS_PATH = pth;
}
//TODO 处理三个参数的情况
void solve(const char *arg1, const char *arg2) {
    //todo 初始化io逻辑
    InitInputFileInfo();
    InitImgs();
    InitCategories();
    string text;
    string category;
    InitPostPath();
    POSTS_PATH /= "content";
    POSTS_PATH /= "posts";
    POSTS_PATH /= string(arg1)+".md";
    if (filesystem::exists(POSTS_PATH))
        exit_print("file exist!");
    InitOutputFileInfo(POSTS_PATH.string());

    //todo 替换文本
    srand(now);                         //以当前时间作为种子重新播种
    int randomIndex = rand() % imgs.size();//随机选择一张图片
    readText.readAll(text);
    to_replace(text, "%s", arg1);//更换文章名字为标题名称
    to_replace(text, "%T", imgs[randomIndex]);//将图片内容进行替换
    to_replace(text, "%D", get_cur_time());   //将当前日期进行替换
    //根据arg2来选择替换的分类内容
    if (to_string(atoi(arg2)) == arg2) {//判断arg2传递的是否是数字
        int index = atoi(arg2);
        if (index < categories.size() - 1) {//数字合法选择对应下标的分类
            category = categories[index];
        } else {//数字不合法，退出程序
            exit_print("number not allowed");
        }
    } else {//不是数字，则说明创建了新的分类
        category = arg2;
        appendCategories.append(string(arg2) + "\n");
    }
    to_replace(text, "#", category);
    //todo 最后再将数据写入磁盘
    fileWriter.write(text);
}

int main(int argc, char const *argv[]) {
    system("chcp 65001");
    SHOW_ARGS state;//由于switch语句的第一层不能定义临时变量
    switch (argc) {
        case 2://todo 外界传来的字符串实际上不是很重要，主要是可以根据这个字符串确定枚举状态，然后再进行不同内容的show操作
            state = MAP[argv[1]];
            if (state == SHOW_ARGS::EMPTY) {//1.哈希表中无该字符串
                fputs("args error,may be you should get something below:\n",stderr);
                for (auto[k, v]:MAP) {//显示参数提示
                    if (v == SHOW_ARGS::EMPTY)continue;//把上面获得值的时候创建的对象给跳过
                    fputs( (k + '\n').c_str(),stderr);
                }
                exit(1);
            } else if (state == SHOW_ARGS::CATEGORIES) {//2.显示分类信息
                InitInputFileInfo();
                InitCategories();
                show(categories);
            } else if (state == SHOW_ARGS::IMG) {//3.显示图片的链接
                InitInputFileInfo();
                InitImgs();
                show(imgs);
            }else if(state == SHOW_ARGS::BLOG_PATH){//4.显示博客地址
                InitPostPath();
                cout<<POSTS_PATH<<'\n';
            }
            return 0;//仅仅展示数据就退出
        case 3:
            solve(argv[1], argv[2]);
            break;
        default:
            printf("Usage :\n%s [title name] [category name]\n", argv[0]);
            printf("%s [-op]\nsuch as -s to show what category number can choose.\n", argv[0]);
            exit(1);
    }
    ifstream reader;//读取编辑器的地址并打开
    reader.open("./ed_Path.txt");
    if(!reader.is_open()){
        exit_print("file reader failed in read editorPath");
    }
    string str;
    reader>>str;
    if(!str.empty())
        open_exe_from_path(str.c_str());
    else{   //如果文件编辑器路径未设置，则默认用vscode打开目录
        string cmd = "code "+POSTS_PATH.string();
        system(cmd.c_str());
        printf("open vscode successfully!\n");
    }
    reader.close();
    cout << "finish yet!" << endl;
    return 0;
}