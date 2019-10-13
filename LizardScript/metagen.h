#pragma once
#include "TypeInfo.h"
#include "VectorsTuple.h"

#define FIELD(field) rawFieldInfo(&(sample->field), #field)
#define FUNC(cl, field) rawFieldInfo(&cl::field, #field)
#define PARAMS(...) WithParams<__VA_ARGS__>

#define METAGEN(className, ...) ([](className* sample) { createMetadata((className*)nullptr, __VA_ARGS__); })(nullptr)

namespace LizardScript
{
	class Dummy
	{
		virtual void* dummy() { return nullptr; }
	};

	class Dummy1 : public Dummy
	{
		virtual void* dummy() override { return nullptr; }
	};

	struct AbstractCallStruct
	{
		virtual void call(void* registers[16], int n) { }
	};

	struct DummyCallStruct : public AbstractCallStruct
	{
		void*(Dummy::*funcptr)();
		virtual void call(void* registers[16], int n) override { }
	};

	template <typename O, typename R, typename... A>
	struct CallStruct : public AbstractCallStruct
	{
		R(O::*funcptr)(A...);
		virtual void call(void* registers[16], int n) override
		{
			int i = sizeof...(A);
			O* ths = (O*)(registers[n]);
			((*ths).*funcptr)((*(A*)&(registers[n + i--]))...);
		}
	};

	template <typename O, typename... A>
	struct CtorProvider
	{
		void ctor(A... args)
		{
			new ((O*)this) O(args...);
		}
	};

	struct MemberInfo
	{
		TypeInfo type;
		std::string name;
	};

	struct FieldInfo : public MemberInfo
	{
		int offset;
	};

	struct FunctionInfo : public MemberInfo
	{
		//only for functions
		char callStruct[sizeof(DummyCallStruct)];
		//TypeInfo returnType;
		std::vector<TypeInfo> args;
	};

	struct TypeInfoEx : public TypeInfo
	{
		TypeInfoEx() { }

		TypeInfoEx(const TypeInfo& from)
		{
			*(TypeInfo*)(this) = from;
		}

		void(*printFunction)(void*);

		VectorsTuple<FieldInfo, FunctionInfo> members;

		//std::vector<FieldInfo> fields;
		//std::vector<FunctionInfo> functions;
	};

	extern std::map<TypeInfo, TypeInfoEx> globalMetadataTable;
}

#include "Print.h"

namespace LizardScript
{
	template <typename T>
	struct RawFieldInfo
	{
		void* offset;
		void*(Dummy::*ptr)();
		std::string name;
	};

	template <typename T>
	inline RawFieldInfo<T> rawFieldInfo(T* ptr, std::string name)
	{
		return RawFieldInfo<T> { (void*)ptr, nullptr, name };
	}

	struct buf
	{
		char buffer[32];
	};

	template <typename R, typename O, typename... A>
	inline RawFieldInfo<R(O::*)(A...)> rawFieldInfo(R(O::*ptr)(A...), std::string name)
	{
		buf buffer;
		*reinterpret_cast<R(O::**)(A...)>(&buffer) = ptr;

		return RawFieldInfo<R(O::*)(A...)> { (void*)-1, *reinterpret_cast<void*(Dummy::**)()>(&buffer), name };
	}

	template <typename... A>
	struct WithParams
	{
		template <typename R, typename O>
		static inline RawFieldInfo<R(O::*)(A...)> rawFieldInfo(R(O::*ptr)(A...), std::string name)
		{
			buf buffer;
			*reinterpret_cast<R(O::**)(A...)>(&buffer) = ptr;

			return RawFieldInfo<R(O::*)(A...)> { (void*)-1, *reinterpret_cast<void*(Dummy::**)()>(&buffer), name };
		}
	};

	template <typename T>
	inline FieldInfo createMetadataEntry(RawFieldInfo<T> info)
	{
		FieldInfo f;
		f.type = makeTypeInfo<T>();
		f.name = info.name;
		f.offset = (int)info.offset;
		return f;
	}

	template <typename O, typename R, typename... A>
	inline FunctionInfo createMetadataEntry(RawFieldInfo<R(O::*)(A...)> info)
	{
		CallStruct<O, R, A...> callStruct;

		buf buffer;
		*reinterpret_cast<void*(Dummy::**)()>(&buffer) = info.ptr;

		callStruct.funcptr = *reinterpret_cast<R(O::**)(A...)>(&buffer);
		FunctionInfo f;
		f.type = makeTypeInfo<R>();

		f.args = { (makeTypeInfo<A>())... };

		f.name = info.name;
		memcpy(&f.callStruct, &callStruct, sizeof(callStruct));
		return f;
	}

	//������� ����� ������
	template <typename O, typename... T>
	inline void createMetadata(O* object)
	{
		std::vector<FieldInfo> metaTable;

		TypeInfo info = makeTypeInfo<O>();
		TypeInfoEx einfo = TypeInfoEx(info);
		einfo.printFunction = &print_r<O>;

		globalMetadataTable.insert(std::make_pair(info, einfo));
	}

	template <typename O, typename... T>
	inline void createMetadata(O* object, T... infos)
	{
		//std::vector<FieldInfo> metaTable;

		TypeInfo info = makeTypeInfo<O>();
		TypeInfoEx einfo = TypeInfoEx(info);
		einfo.printFunction = &print_r<O>;
		//einfo.fields = metaTable;
		char c[] = { (einfo.members.push_back(createMetadataEntry(infos)), '\0')... };

		globalMetadataTable.insert(std::make_pair(info, einfo));
	}
}
