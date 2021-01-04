#include "pch.h"
#include "win_shared_memory.h"
_IPC_BEGIN


SharedMemory::SharedMemory(const std::wstring name, uint32_t len) :
	shared_memory_name(name),
	shared_memory_len(len) {
}

SharedMemory::~SharedMemory() {
	this->close();
}

auto SharedMemory::create() ->HRESULT {
	this->mapping_handle = CreateFileMappingW(
		nullptr,
		nullptr,
		PAGE_READWRITE,
		0,
		this->shared_memory_len,
		this->shared_memory_name.c_str()
	);
	if (this->mapping_handle == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}

	this->shared_memory_ptr = (uint8_t*)MapViewOfFile(
		this->mapping_handle,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		this->shared_memory_len
	);
	if (this->shared_memory_ptr == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	return S_OK;
}

auto SharedMemory::open() ->HRESULT {
	this->mapping_handle = OpenFileMappingW(
		FILE_MAP_ALL_ACCESS,
		FALSE,
		this->shared_memory_name.c_str()
	);
	if (this->mapping_handle == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	this->shared_memory_ptr = (uint8_t*)MapViewOfFile(
		mapping_handle,
		FILE_MAP_ALL_ACCESS,
		0,
		0,
		this->shared_memory_len
	);

	if (this->shared_memory_ptr == nullptr) {
		return HRESULT_FROM_WIN32(GetLastError());
	}
	return S_OK;
}

uint8_t* SharedMemory::ptr() {
	return this->shared_memory_ptr;
}

auto SharedMemory::length() ->uint32_t {
	if (ptr() == nullptr) {
		return 0;
	}
	else {
		return this->shared_memory_len;
	}
}

auto SharedMemory::close() ->void {
	if (this->shared_memory_ptr) {
		UnmapViewOfFile(this->shared_memory_ptr);
		this->shared_memory_ptr = nullptr;
	}
	if (this->mapping_handle) {
		CloseHandle(this->mapping_handle);
		this->mapping_handle = nullptr;
	}
}

_IPC_END
