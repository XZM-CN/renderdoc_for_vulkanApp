#ifndef __FileUtil_H__
#define __FileUtil_H__
#include <string>
using namespace std;
typedef struct SpvDataStruct
{                   //�洢SPIR-V ���ݵĽṹ��
    int size;       //SPIR-V �������ֽ���
    uint32_t *data; //ָ��SPIR-V �����ڴ���׵�ַ��ָ��
} SpvData;
class FileUtil
{
public:
    static string loadAssetStr(string fname);
    static SpvData &loadSPV(string fname); //�����ļ����µ�SPIR-V ����
};
#endif
