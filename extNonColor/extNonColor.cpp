// extNonColor.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "3rd/zlib.h"
#include <iostream>
#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <string>
#include <vector>
using namespace std;

#pragma pack (1)
struct DatEntry
{
	DWORD hash;
	BYTE  IsCompress;
	DWORD Offset;
	DWORD PackedSize;
	DWORD UnpackSize;
};
#pragma pack ()

int E(const char* msg)
{
	MessageBoxA(0, msg, 0, 0);
	return -1;
}

DWORD swap_to_le(DWORD x)
{
	int y;
	char* a = (char*)&y;
	a[0] = x >> 24;
	a[1] = x >> 16;
	a[2] = x >> 8;
	a[3] = x;
	return y;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		MessageBox(0, L"extNonColor.exe <input dat file>", L"AyamiKaze", 0);
		return -1;
	}
	CHAR* FileName = argv[1];
	FILE* fp = fopen(FileName, "rb");
	DWORD Count = 0;
	fread(&Count, 1, 4, fp);

	Count = swap_to_le(Count);
	cout << "Count:0x" << hex << Count << endl;
	
	DatEntry* Entry = new DatEntry[sizeof(DatEntry) * Count];
	fread(Entry, 1, sizeof(DatEntry) * Count, fp);

	FILE* idx = fopen("idx.bin", "wb");
	fwrite(Entry, 1, sizeof(DatEntry) * Count, idx);
	fclose(idx);

	for (DWORD i = 0; i < Count; i++)
	{
		DWORD hash = swap_to_le(Entry[i].hash);
		DWORD Offset = swap_to_le(Entry[i].Offset);
		DWORD PackedSize = swap_to_le(Entry[i].PackedSize);
		DWORD UnpackSize = swap_to_le(Entry[i].UnpackSize);

		cout << "---------------------------------------" << endl;
		cout << "hash:0x" << hex << hash << endl;
		cout << "Offset:0x" << hex << Offset << endl;
		cout << "PackedSize:0x" << hex << PackedSize << endl;
		cout << "UnpackSize:0x" << hex << UnpackSize << endl;

		BYTE* FileBuff = new BYTE[PackedSize];
		fseek(fp, Offset, SEEK_SET);
		fread(FileBuff, 1, PackedSize, fp);

		BYTE* DstBuff = nullptr;
		DWORD DstSize = 0;
		if (PackedSize != UnpackSize)
		{
			cout << "uncompress" << endl;
			DstBuff = new BYTE[UnpackSize];
			if (uncompress(DstBuff, &UnpackSize, FileBuff, PackedSize) != Z_OK)
				return E("zlib uncompress error");
			delete[] FileBuff;
			DstSize = UnpackSize;
		}
		else 
		{
			DstBuff = FileBuff;
			DstSize = PackedSize;
		}

		char OutName[MAX_PATH];
		sprintf(OutName, "output\\%x", hash);
		cout << "OutName:" << OutName << endl;
		FILE* fout = fopen(OutName, "wb");
		fwrite(DstBuff, 1, DstSize, fout);
		fclose(fout);
		cout << "done" << endl;
	}

	fclose(fp);
	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
