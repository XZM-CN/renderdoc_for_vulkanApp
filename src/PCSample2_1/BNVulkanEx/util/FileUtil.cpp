#include "FileUtil.h"
#include <fstream>
#include <string>
#include <iostream>
#include <assert.h>
#include <stdio.h>
int getfilesize(string fname)
{
    FILE *fp;
    if ((fp = fopen(fname.c_str(), "r")) == NULL)
        return 0;
    fseek(fp, 0, SEEK_END);
    return ftell(fp);
}
string FileUtil::loadAssetStr(string fname)
{
    ifstream infile;
    infile.open(fname.data());
    cout << "fname>" << fname << endl;
    assert(infile.is_open());
    string ss;
    string s;
    while (getline(infile, s))
    {
        ss += s;
        ss += '\n';
    }
    infile.close();
    return ss;
}
SpvData &FileUtil::loadSPV(string fname) //加载文件夹下的SPIR-V 数据文件
{
    size_t size = (getfilesize(fname)); //获取SPIR-V 数据文件的总字节数
    cout << "len:" << size << endl;
    SpvData spvData;                           //构建SpvData 结构体实例
    spvData.size = size;                       //设置SPIR-V 数据总字节数
    spvData.data = (uint32_t *)(malloc(size)); //分配相应字节数的内存
    char *buf    = (char *)spvData.data;       //从文件中加载数据进入内存
    char c_file[1000];
    strcpy(c_file, fname.c_str());
    FILE *fpSPV;
    fpSPV = fopen(c_file, "rb");
    if (fpSPV == NULL)
    {
        printf("打开文件%s失败\n", fname.c_str());
    }
    fread(buf, size, 1, fpSPV);
    return spvData; //返回SpvData 结构体实例
}
