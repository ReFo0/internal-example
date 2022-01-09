#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include <memory>
#include <tchar.h>
#include <array>

uintptr_t module_base(LPCSTR a_module) {
	GetModuleHandleA(a_module);
	return NULL;
}

uintptr_t examplemodule = module_base("example.dll");

bool readsize(uint64_t address, void* buffer, SIZE_T size)
{
	if (!address)
		return false;

	memcpy(buffer, reinterpret_cast<const void*>(address), size);
	return true;
}

BOOL valid_pointer(DWORD64 address)
{
	if (!IsBadWritePtr((LPVOID)address, (UINT_PTR)8)) return TRUE;
	else return FALSE;
}

template<typename ReadT>
ReadT read(DWORD_PTR address, const ReadT& def = ReadT())
{
	if (valid_pointer(address)) {
		return *(ReadT*)(address);
	}
}

template<typename WriteT>
bool write(DWORD_PTR address, WriteT value, const WriteT& def = WriteT())
{
	if (valid_pointer(address)) {
		*(WriteT*)(address) = value;
		return true;
	}
	return false;
}

std::uintptr_t find_signature(const std::uintptr_t base_address, const char* sig, const char* mask)
{
	const auto buffer = std::make_unique<std::array<std::uint8_t, 0x100000>>();
	auto data = buffer.get()->data();
	std::uintptr_t result = 0;

	for (std::uintptr_t i = 0u; i < (2u << 25u); ++i)
	{
		readsize(base_address + i * 0x100000, data, 0x100000);

		if (!data)
			return 0;

		for (std::uintptr_t j = 0; j < 0x100000u; ++j)
		{
			if ([](std::uint8_t const* data, std::uint8_t const* sig, char const* mask)
				{
					for (; *mask; ++mask, ++data, ++sig)
					{
						if (*mask == 'x' && *data != *sig) return false;
					}
					return (*mask) == 0;
				}(data + j, (std::uint8_t*)sig, mask))
			{
				result = base_address + i * 0x100000 + j;

				std::uint32_t rel = 0;

				readsize(result + 3, &rel, sizeof(std::uint32_t));

				if (!rel)
					return 0;

				return result - base_address + rel + 7;
			}
		}
	}
	return 0;
}

void show_console() {
	AllocConsole();
	freopen("CONIN$", "r", stdin);
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
}
