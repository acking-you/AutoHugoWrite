## 使用教程

[参考视频教程](#)

## 项目介绍

> 温馨提示：如果本地还未搭建 hugo 博客，可以使用我的另一个 [hugo 博客自动搭建工具](https://github.com/ACking-you/AutoHugoSetup)

![AutoHugoWrite](https://s2.loli.net/2022/02/09/yJY4VAImClOKcqE.png)

* **QtRun**：

  * **介绍**：一个用纯 C++ 写的命令行工具。

  * **主要作用**：根据提供的 hugo 博客本地地址进行命令行式的自动化写作，会把每一篇文章的图片、分类、标题等内容自动化完成。

  * **构建方式**：确保生成 exe 文件的目录下含有以下文件，且确保编译器支持 C++17。

    ```shell
    BlogPath.txt #提供本地hugo博客路径
    categories.txt #提供可供选择的分类(没有也没关系)
    initImg.txt		#提供可选择的图片
    mob.txt			#提供用于生成的模板
    ed_Path.txt		#提供打开的编辑器路径(没有也没关系)
    ```

  * **使用方式**：可查看源码得到更详尽的解答

    ```shell
     QtRun [title name] [category name]
     Qtrun [-op]
    ```

    

* **QtRunBlog**：
  * **介绍**：使用 Qt+cmake 搭建的图形化 hugo 自动化工具，写作的相关部分都是调用的 QtRun ，所以运行时 QtRun 的配置文件和 QtRun 都必须在它的 exe 目录之下。而其他其他部分调用的 git 命令行，所以需要本地有 git 工具。
  * **主要作用**：提供图形化的 hugo 写作体验。
  * **构建方式**：本地需要 Qt6 环境，选择本项目目录便可完成构建。
