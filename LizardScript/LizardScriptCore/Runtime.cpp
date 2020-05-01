#include "pch.h"
#include "Runtime.h"
#include "RuntimeMacros.h"

using namespace LizardScript;

void Runtime::run(const LsFunction& f)
{
	//4 ����� - last frame LsFunction
	//4 ����� - last frame EIP
	//4 ����� - last frame ESP			
	//����� ���������
	//����� ��������
	//����� ��������� ����������

	//short frame mode - r1 � r2 �� 1 �����, �������� 256 ���������� ���� �� ���� ����� � ������
	//long frame mode - r1 � r2 �� 2 �����, �� 65536 ���� �� ���� ��� ������

	size_t eip = 0;
	size_t esp = 0;

	using OFFSET_T = unsigned char;

	using byte = unsigned char;
	using uint = unsigned int;

	using int8 = char;
	using int16 = short;
	using int32 = int;
	using int64 = long long;

	using uint8 = unsigned char;
	using uint16 = unsigned short;
	using uint32 = unsigned int;
	using uint64 = unsigned long long;

	while (true)
	{
		LsCode code = CODEGET(LsCode);
		OFFSET_T r1 = CODEGET(OFFSET_T);
		OFFSET_T r2 = CODEGET(OFFSET_T);

		switch (code)
		{
#include "RuntimeCases.i"
		}
	}
}
