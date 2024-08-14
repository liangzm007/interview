#include <iostream>
#include <list>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <algorithm>

using namespace std;

class Findupdate
{
public:

    /*
    函数说明：对目录内的以 stp 为后缀名的文件进行排序重命名
    参数：
        path：指定的目录绝对路径
    返回值：
        如果成功返回真，否则返回假
    */
    bool checkFile(std::string path)
    {
        if (!getFiles(path, "stp", false, _result))
        {
            return false;
        }

        _result.sort([](std::string& file1, std::string& file2) {
            for (int i = 0, c1 = file1.size(), c2 = file2.size(); i < c1 && i < c2; ++i)
            {
                if (file1.at(i) < file2.at(i))
                {
                    return true;
                }
            }

            return file1.size() < file2.size();
        });

        return AddFileNameIndex(_result);
    }

    /*
    函数说明：获取文件排序重命名后的结果，一个文件路径信息的列表
    参数：
        无
     返回值：
        返回包含文件路径的列表，否则返回空列表
     备注：
        此函数需 checkFile 返回真的情况下才有意义
    */
    inline std::list<std::string> getResult() { return _result; }

private:

    /*
    函数说明：获取目录内的指定后缀名的文件，如果未指定后缀名，则获取目录下的所有文件
    参数：
        path：  指定的目录绝对路径
        suffix: 指定的文件后缀名，可为空，如果为空，则获取目录下的所有文件
        result：结果列表
    返回值：
        如果成功返回真，否则返回假
    */
    static bool getFiles(const std::string& path, const std::string& suffix, bool recursion, std::list<std::string>& result)
    {
        if (path.size() == 0)
        {
            return false;
        }

        DIR* dir = opendir(path.c_str());
        if (!dir)
        {
            return false;
        }

        const char  dotName = '.';
        std::string dotPath = ".";
        std::string dotDotPath = "..";

        struct dirent* dt = nullptr;
        while ((dt = readdir(dir)) != nullptr)
        {
            std::string filePath = path;
            if (path.back() != '/')
            {
                filePath.append("/");
            }

            filePath.append(dt->d_name);

            if (dt->d_type == DT_DIR)
            {
                if (dotPath.compare(dt->d_name) == 0 || dotDotPath.compare(dt->d_name) == 0)
                {
                    continue;
                }

                if (recursion)
                {
                    if (!checkFile(filePath, suffix, recursion, result))
                    {
                        return false;
                    }
                }
            }
            else if (dt->d_type == DT_REG)
            {
                if (suffix.size() == 0)
                {
                    result.push_back(filePath);
                }
                else
                {
                    char* pos = strrchr(dt->d_name, dotName);
                    if (!pos || *(pos + 1) == '\0')
                    {
                        continue;
                    }

                    if (suffix.compare(pos + 1) == 0)
                    {
                        result.push_back(filePath);
                    }
                }
            }
        }

        closedir(dir);

        return true;
    }

    /*
    函数说明：对列表内的文件名添加序列号，并重命名
    参数：
        filePath：文件路径列表
    返回值：
        如果成功返回真，否则返回假
    */
    static bool AddFileNameIndex(std::list<std::string>& filePath)
    {
        if (filePath.size() == 0)
        {
            return true;
        }

        int index = 1;
        for (auto iter = filePath.begin(); iter != filePath.end(); ++iter)
        {
            const char* path = iter->c_str();
            const char* name = strrchr(path, '/');
            if (!name || *(++name) == '\0')
            {
                return false;
            }

            char basePath[1024] = {0};
            memcpy(basePath, path, iter->size());
            basePath[name - path] = '\0';

            const char* pos = strrchr(name, '.');
            //
            if (!pos || *(++pos) == '\0')
            {
                pos = nullptr;
            }

            char baseName[1024] = {0};
            int len = strlen(name);
            memcpy(baseName, name, len);

            const char* suffix = nullptr;
            if (pos)
            {
                suffix = pos;
                baseName[suffix - name - 1] = '\0';
            }

            char fullPath[1024] = {0};
            if (suffix)
            {
                snprintf(fullPath, sizeof(fullPath), "%s%s-%02d.%s", basePath, baseName, index++, suffix);
            }
            else
            {
                snprintf(fullPath, sizeof(fullPath), "%s%s-%02d", basePath, baseName, index++);
            }

            iter = filePath.erase(iter);
            iter = filePath.insert(iter, fullPath);

            if (rename(iter->c_str(), fullPath) != 0)
            {
                return false;
            }
        }

        return true;
    }

private:

    std::list<std::string> _result;
};

int main()
{
    const char* path = "/home/test";
    std::list<std::string> result;

    Findupdate findupdate;
    if (findupdate.checkFile(path))
    {
        result = findupdate.getResult();
    }

    // 输出结果
    for (auto& file : result)
    {
        std::cout << file.c_str() << std::endl;
    }

    return 0;
}
