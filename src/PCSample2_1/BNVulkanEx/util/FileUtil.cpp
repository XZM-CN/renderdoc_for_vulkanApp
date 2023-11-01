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
SpvData &FileUtil::loadSPV(string fname) //�����ļ����µ�SPIR-V �����ļ�
{
    size_t size = (getfilesize(fname)); //��ȡSPIR-V �����ļ������ֽ���
    cout << "len:" << size << endl;
    SpvData spvData;                           //����SpvData �ṹ��ʵ��
    spvData.size = size;                       //����SPIR-V �������ֽ���
    spvData.data = (uint32_t *)(malloc(size)); //������Ӧ�ֽ������ڴ�
    char *buf    = (char *)spvData.data;       //���ļ��м������ݽ����ڴ�
    char c_file[1000];
    strcpy(c_file, fname.c_str());
    FILE *fpSPV;
    fpSPV = fopen(c_file, "rb");
    if (fpSPV == NULL)
    {
        printf("���ļ�%sʧ��\n", fname.c_str());
    }
    fread(buf, size, 1, fpSPV);
    return spvData; //����SpvData �ṹ��ʵ��
}
