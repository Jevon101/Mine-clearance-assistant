/*
* @title:Mine-clearance-assistant
* @autor:Jev0n
* @date:2020.10.03
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <TlHelp32.h>


int main(int argc, char* argv[])
{
	printf("*****ɨ�����׳�������******\n");

	int i = 0,k = 0;
	HWND hWnd = 0;
	DWORD Pid = 0;
	HANDLE hProcess = 0;
	HANDLE hProcesSapan = 0;
	DWORD dwInfo = 0x01005330;
	DWORD timeAddress = 0x0100579C;
	DWORD incAddress1 = 0x01002FF5;
	DWORD incAddress2 = 0x01003830;
	DWORD MinefieldsAddress = 0x01005340;
	DWORD MinefieldsSize = 832;
	
	DWORD dwFlag = 0x8E;
	DWORD dwHeight = 0, dwWidth = 0;
	DWORD time = 0;
	PROCESSENTRY32 pe32 = { 0 };
	char inc[6] = { 0x90,0x90,0x90,0x90,0x90,0x90 };

	PBYTE pByte = NULL;
	pByte = (PBYTE)malloc(MinefieldsSize);

	pe32.dwSize = sizeof(PROCESSENTRY32);


	//�򿪽���
	hProcesSapan = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if (hProcesSapan == INVALID_HANDLE_VALUE)
	{
		printf_s("CreatToolhelp32Snapshot Error!\r\n");
		return -1;
	}
	BOOL bRet = Process32First(hProcesSapan, &pe32);
	while (bRet)
	{
		if (!wcscmp(pe32.szExeFile, L"winmine.exe"))
		{
			Pid = pe32.th32ProcessID;
			break;
		}
		bRet = Process32Next(hProcesSapan, &pe32);
	}
	if (!Pid)
	{
		printf("\n�Ҳ���ɨ�׽��̣���������´򿪱�����\r\n");
		return -1;
	}
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
	if (!hProcess)
	{
		printf("�򿪽���ʧ�ܣ�\r\n");
		return -1;
	}



	//�޸�ʱ������
	if (!WriteProcessMemory(hProcess,(LPVOID)timeAddress,&time,2,NULL))
	{
		printf("ʱ���޸�ʧ�ܣ��������Ϊ %d",GetLastError());
		return -1;
	}
	if (!WriteProcessMemory(hProcess,(LPVOID)incAddress1,&inc,6,NULL) || !WriteProcessMemory(hProcess, (LPVOID)incAddress2, &inc, 6, NULL))
	{
		printf("ʱ����������޸�ʧ�ܣ��������Ϊ %d", GetLastError());
		return -1;
	}

	//��ȡ������Ϣ
	ReadProcessMemory(hProcess, (LPVOID)MinefieldsAddress, pByte, MinefieldsSize, NULL);
	while (i < MinefieldsSize)
	{
		if (pByte[i] == 0x8F)
		{
			DWORD tmpAddress = MinefieldsAddress + i;
			WriteProcessMemory(hProcess, (LPVOID)tmpAddress, &dwFlag, sizeof(BYTE), NULL);
		}
		i++;
	}

	// ˢ��ɨ�׵Ŀͻ���
	RECT rt;
	hWnd = FindWindow(NULL, L"ɨ��");
	if (!hWnd)
	{
		printf("hWnd Error!");
		return -1;
	}
	GetClientRect(hWnd, &rt);
	InvalidateRect(hWnd, &rt, TRUE);



	//��ȡ����
	ReadProcessMemory(hProcess, (LPVOID)(dwInfo + 4), &dwWidth, sizeof(DWORD), 0);
	ReadProcessMemory(hProcess, (LPVOID)(dwInfo + 8), &dwHeight, sizeof(DWORD), 0);
	
	//��ȡ����λ��
	int h = dwHeight;
	int cnt = 0;
	i = 0;
	PBYTE pTmpByte = NULL;
	pTmpByte = (PBYTE)malloc(dwHeight * dwWidth);

	while (i < MinefieldsSize)
	{
		
		if (pByte[i] == 0x10 && pByte[i + 1] == 0x10)
		{
			i = i + dwWidth + 2;
			continue;
		}
		else if (pByte[i] == 0x10)
		{
			for (k = 1; k <= dwWidth; k++)
			{
				pTmpByte[cnt] = pByte[i + k];
				cnt++;
			}
			i = i + dwWidth + 2;
			continue;
			h--;
			if (h == 0) 
				break;
		}
		i++;
	}



	//��Ϣ����ʵ��һ��ɨ��
	int x1 = 0, y1 = 0;
	int x = 0, y = 0;
	for (i = 0; i < dwHeight * dwWidth; i++) 
	{
		if (pTmpByte[i] != 0x8F) 
		{
			x1 = i % dwWidth;
			y1 = i / dwWidth;
			x = x1 * 16 + 16;
			y = y1 * 16 + 61;
			SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELONG(x, y));
			Sleep(1);
			SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELONG(x, y));
		}
	}

	

	CloseHandle(hProcess);
	CloseHandle(hProcesSapan);
	free(pByte);

	system("pause");
	return 0;
}
