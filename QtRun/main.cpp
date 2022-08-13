//
// Created by Alone on 2022-1-24.
//

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


using namespace std;
enum class SHOW_ARGS : int {
    EMPTY, CATEGORIES, IMG, BLOG_PATH
};

unordered_map<string, SHOW_ARGS> MAP{
        {"-sc", SHOW_ARGS::CATEGORIES},
        {"-si", SHOW_ARGS::IMG},
        {"-sp", SHOW_ARGS::BLOG_PATH}
};

class QtRun {
public:
    QtRun() {
        try {
            init();
        } catch (exception const &e) {
            cout << e.what() << endl;
            exit(1);
        }
    }

    static QtRun &GetInstance() {
        static QtRun instance;
        return instance;
    }

    vector<string> &Categories() {
        return m_categories;
    }

    vector<string> &Images() {
        return m_imgPaths;
    }

    string &BlogPath() {
        return m_blogPath;
    }

    static void OpenExeFromPath(string_view path) {
        WinExec(path.data(), SW_SHOWDEFAULT);
        cout << "open your custom editor successfully!" << endl;
    }

    void ShowImags() {
        show(m_imgPaths);
    }

    void ShowCategories() {
        show(m_categories);
    }

    void ShowBlogPath() {
        cout << m_blogPath << endl;
    }

    void StartWithTitleAndCategory(const char *title, const char *category) {
        if (!start_with_title_category(title, category)) {
            cout << "start failed,maybe you don't have some folder" << endl;
            exit(1);
        }
    }

private:
    bool start_with_title_category(string_view title, string_view category) {
        replace_all(m_mob, "%s", title);//替换标题和tag
        replace_all(m_mob, "%D", get_cur_time()); //替换时间
        srand((unsigned int) m_now);
        replace_all(m_mob, "%T", m_imgPaths[rand() % m_imgPaths.size()]); //获取一张随机的图片

        size_t index = strtol(category.data(), nullptr, 10);
        bool check_exist = false;
        if (index > 0 && index <= m_categories.size()) { //若index > 0则说明有用序号
            check_exist = true;
            category = std::move(m_categories[index]);
        }
        replace_all(m_mob, "#{categories}", category); //替换分类
        if (!check_exist) //若不存在，则向文件中写入
        {
            ofstream ofs(CATEGORIES_PATH, ios::app);
            if (!ofs) {
                return false;
            }
            ofs << "\r\n" << category;
        }
        //开始写入对应的文章
        filesystem::path out_path = m_blogPath;
        out_path /= "content";
        if (!filesystem::exists(out_path)) {
            return false;
        }
        out_path /= "posts";
        if (!filesystem::exists(out_path)) {
            return false;
        }
        m_blogPath = std::move(out_path.string()); //更新路径

        //以文章title为文件名
        out_path /= title;
        ofstream ofs(out_path.string() + ".md");
        if (!ofs) {
            return false;
        }
        ofs << m_mob;
        return true;
    }

    static void show(vector<string> const &src) {
        int index = 1;
        for (auto &&p: src) {
            if (!p.empty()) {
                printf("%d: %s\r\n", index, p.data());
                index++;
            }
        }
    }

    void init() {
        //init imags
        {
            ifstream ifs(IMGS_PATH);
            if (!ifs) {
                string info = "initImg.txt not exist! current_path:";
                info += filesystem::current_path().string();
                throw std::runtime_error(info);
            }
            string line; //初始化imgs
            while (getline(ifs, line)) {
                trim(line);
                if (!line.empty())
                    m_imgPaths.push_back(std::move(line));
            }
        }
        //init categories
        {
            ifstream ifs(CATEGORIES_PATH);
            if (!ifs) {
                throw std::runtime_error("categories.txt not exist!");
            }
            string line; //初始化categories
            while (getline(ifs, line)) {
                trim(line);
                if (!line.empty())
                    m_categories.push_back(std::move(line));
            }
        }
        //init blogpath
        {
            ifstream ifs(BLOG_SRC);
            if (!ifs) {
                throw std::runtime_error("BlogPath.txt not exist!");
            }
            m_blogPath = std::move(string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>()));
            //去除左右两边的空格
            trim(m_blogPath);
        }
        //init mob
        {
            ifstream ifs(MOB_PATH);
            if (!ifs) {
                throw std::runtime_error("mob.txt not exist!");
            }
            m_mob = std::move(string(istreambuf_iterator<char>(ifs), istreambuf_iterator<char>()));
        }

        //初始时间
        m_now = time(nullptr);
    }

    static void replace_all(string &str, string_view obj, string_view replacement) {
        auto iter = str.find(obj);
        while (iter != string::npos) {
            str.replace(iter, obj.size(), replacement);
            iter = str.find(obj, iter);
        }
    }

    static void trim(string &src) {
        src.erase(src.begin(), std::find_if(src.begin(), src.end(), [](char x) {
            return !isspace(x);
        }));
        src.erase(std::find_if(src.rbegin(), src.rend(), [](char x) {
            return !isspace(x);
        }).base(), src.end());
    }

    string get_cur_time() {
        tm *tm_t = localtime(&m_now);
        char c_time[50];
        sprintf(c_time, "%04d-%02d-%02d",
                tm_t->tm_year + 1900,
                tm_t->tm_mon + 1,
                tm_t->tm_mday);
        return c_time;
    }

private:
    time_t m_now{};
    string m_mob;
    string m_blogPath;
    vector<string> m_imgPaths;
    vector<string> m_categories;
};


int main(int argc, char const *argv[]) {

    SHOW_ARGS state;//由于switch语句的第一层不能定义临时变量
    switch (argc) {
        case 2:
            state = MAP[argv[1]];
            if (state == SHOW_ARGS::EMPTY) {//1.哈希表中无该字符串
                fputs("args error,may be you should get something below:\n", stderr);
                for (auto [k, v]: MAP) {//显示参数提示
                    if (v == SHOW_ARGS::EMPTY)continue;//把上面获得值的时候创建的对象给跳过
                    fputs((k + '\n').c_str(), stderr);
                }
                exit(1);
            } else if (state == SHOW_ARGS::CATEGORIES) {//2.显示分类信息
                QtRun::GetInstance().ShowCategories();
            } else if (state == SHOW_ARGS::IMG) {//3.显示图片的链接
                QtRun::GetInstance().ShowImags();
            } else if (state == SHOW_ARGS::BLOG_PATH) {//4.显示博客地址
                QtRun::GetInstance().ShowBlogPath();
            }
            return 0;//仅仅展示数据就退出
        case 3:
            QtRun::GetInstance().StartWithTitleAndCategory(argv[1], argv[2]);
            break;
        default:
            printf("Usage :\n%s [title name] [category name]\n", argv[0]);
            printf("%s [-op]\nsuch as -s to show what category number can choose.\n", argv[0]);
            fflush(stdout);
            exit(1);
    }
    ifstream reader;//读取编辑器的地址并打开
    reader.open("./ed_Path.txt");
    if (!reader) {
        cout << "file reader failed in read editorPath" << endl;
        exit(1);
    }
    string str;
    reader >> str;
    if (!str.empty())
        QtRun::OpenExeFromPath(str.c_str());
    else {   //如果文件编辑器路径未设置，则默认用vscode打开目录
        string cmd = "code " + QtRun::GetInstance().BlogPath();
        system(cmd.c_str());
        printf("open vscode successfully!\n");
    }
    reader.close();
    cout << "finish yet!" << endl;
    return 0;
}
