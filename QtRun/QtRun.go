package main

import (
	"bufio"
	"errors"
	"flag"
	"fmt"
	"io"
	"io/ioutil"
	"math/rand"
	"os"
	"os/exec"
	"path/filepath"
	"runtime"
	"strconv"
	"strings"
	"time"
)

const (
	KImgPath    = "./initImg.txt"
	KMobPath    = "./mob.txt"
	KCategories = "./categories.txt"
	KBlog       = "./BlogPath.txt"
	KEditorPath = "./ed_Path.txt"
)

var (
	sc   bool
	si   bool
	sp   bool
	args []string
)

func initFlag() {
	flag.BoolVar(&sc, "sc", false, "show category")
	flag.BoolVar(&si, "si", false, "show images")
	flag.BoolVar(&sp, "sp", false, "show blog env path")
	recover()
	flag.Parse()
	args = flag.Args()
}

func command(cmd string) (err error) {
	if runtime.GOOS == "windows" {
		err = exec.Command("cmd", "/C", cmd).Start()
	} else {
		err = exec.Command("/bin/bash", "-c", cmd).Start()
	}
	return
}

type StringHelper struct {
	str string
}

func (s *StringHelper) ReplaceAll(old, new string) *StringHelper {
	new = strings.Trim(new, " \r\n")
	s.str = strings.ReplaceAll(s.str, old, new)
	return s
}

func show(list []string) {
	for i, v := range list {
		fmt.Printf("%d: %s\n", i, v)
	}
}

func getTextFromFileByLine(filename string) ([]string, error) {
	fd, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	reader := bufio.NewReader(fd)
	var ret []string
	for {
		line, err := reader.ReadString('\n')
		line = strings.Trim(line, " \r\n")
		if line != "" {
			ret = append(ret, line)
		}
		if errors.Is(io.EOF, err) {
			break
		}
	}
	return ret, err
}

func getAllTextFromFile(filename string) (string, error) {
	fd, err := os.Open(filename)
	if err != nil {
		return "", err
	}
	bytes, err := ioutil.ReadAll(fd)
	return string(bytes), err
}

func showTextByLine(filename string) error {
	txt, err := getTextFromFileByLine(filename)
	if err != nil {
		return err
	}
	show(txt)
	return nil
}

func startWrite(title, category string) error {
	//获取blogPath
	blogPath, err := getAllTextFromFile(KBlog)
	blogPath = strings.Trim(blogPath, "\n \r")
	if err != nil {
		return err
	}
	//获取图片链接
	imgs, err := getTextFromFileByLine(KImgPath)
	if err != nil {
		return err
	}
	//获取分类
	cates, err := getTextFromFileByLine(KCategories)
	if err != nil {
		return err
	}
	//获取当前时间并格式化
	date := time.Now().Format("2006-01-02")
	//获取模板的内容
	mob, err := getAllTextFromFile(KMobPath)
	if err != nil {
		return err
	}
	//更新需要写入的路径
	blogPath = filepath.Join(blogPath, "content", "posts", fmt.Sprintf("%s.md", title))
	helper := StringHelper{str: mob}
	//替换标题和Tag
	helper.ReplaceAll("%s", title)
	//替换时间
	helper.ReplaceAll("%D", date)
	//替换随机一张图片链接
	rand.Seed(time.Now().Unix())
	helper.ReplaceAll("%T", imgs[rand.Intn(len(imgs))])
	//替换分类，如果分类为数字则查看已有分类
	c, err := strconv.Atoi(category)
	if err == nil {
		if c > len(cates) {
			return errors.New("分类索引值无效")
		}
		category = cates[c]
	}
	helper.ReplaceAll("#{categories}", category)

	//写入整个模板的内容
	wfd, err := os.Create(blogPath)
	if err != nil {
		return err
	}
	_, err = wfd.WriteString(helper.str)
	if err != nil {
		return err
	}
	//打开对应的编辑器，如果没有对应的配置，则默认打开vscode
	path, err := getAllTextFromFile(KEditorPath)
	if err != nil {
		fmt.Println("open_editor-err:", err.Error())
		fmt.Println("try open vscode")
		_ = command(fmt.Sprintf("%s %s", "code", blogPath))
	} else {
		_ = command(fmt.Sprintf("%s %s", strings.Trim(path, " \r\n"), blogPath))
		fmt.Println("open editor ok!")
	}
	return nil
}

func main() {
	initFlag()
	if sc {
		err := showTextByLine(KCategories)
		if err != nil {
			fmt.Println("sc-err:", err.Error())
		}
	}
	if si {
		err := showTextByLine(KImgPath)
		if err != nil {
			fmt.Println("si-err:", err.Error())
		}
	}
	if sp {
		err := showTextByLine(KBlog)
		if err != nil {
			fmt.Println("sp-err:", err.Error())
		}
	}

	if len(args) == 2 {
		err := startWrite(args[0], args[1])
		if err != nil {
			fmt.Println("write-err:", err.Error())
		}
	}

}
