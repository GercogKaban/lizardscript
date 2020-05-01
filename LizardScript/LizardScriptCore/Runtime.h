#pragma once
#include <vector>

namespace LizardScript
{
	using LsCode = unsigned char;

	class LsFunction
	{
	public:
		std::vector<unsigned char> code;
	};

	class Runtime
	{
	public:

		unsigned char stack[1024 * 10];

		void run(const LsFunction& f)
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

#define REGISTER(type, i) = stack[esp + i];
			//r == 0 ? *(type*)((char*)stackbase + CODEGET(short)) : *(type*)&(registers[r])
#define CODEGET(type) *(type*)(&f.code[(eip += sizeof(type)) - sizeof(type)])
#define PCODEGET(type) (type*)(&f.code[(eip += sizeof(type)) - sizeof(type)])

#define OPCODE_SET(T1, T2)\
*REGISTER(T1*, r1) = REGISTER(T1, r2);

#define OPCODE_PUSH_CONST(T)\
*REGISTER(T*, r2) = CODEGET(T);

			while (true)
			{
				LsCode code = CODEGET(LsCode);
				OFFSET_T r1 = CODEGET(OFFSET_T);
				OFFSET_T r2 = CODEGET(OFFSET_T);


			}
		}
	};
}