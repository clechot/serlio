#include "util/MayaUtilities.h"

#include <cassert>
#include <memory>

namespace {

void setHandlePosition(adsk::Data::Handle& handle, const wchar_t* key) {
	const std::string nKey = prtu::toOSNarrowFromUTF16(key);
	handle.setPositionByMemberName(nKey.c_str());
}

constexpr size_t MAX_STRING_LENGTH = 400;
constexpr size_t MAX_ARRAY_SIZE = 5;

adsk::Data::Member::eDataType getWideCharacterType() {
#ifdef _WIN32
	static_assert(sizeof(wchar_t) == 2, "wide character size on Windows expected to be 2 bytes");
	return adsk::Data::Member::kUInt16;
#else
	static_assert(sizeof(wchar_t) == 4, "wide character size on Linux expected to be 4 bytes");
	return adsk::Data::Member::kUInt32;
#endif
}

wchar_t* getWideCharacterPtr(adsk::Data::Handle& handle) {
#ifdef _WIN32
	static_assert(sizeof(wchar_t) == 2, "wide character size on Windows expected to be 2 bytes");
	wchar_t* destPtr = reinterpret_cast<wchar_t*>(handle.asUInt16());
#else
	static_assert(sizeof(wchar_t) == 4, "wide character size on Linux expected to be 4 bytes");
	wchar_t* destPtr = reinterpret_cast<wchar_t*>(handle.asUInt32());
#endif
	return destPtr;
}

void copyRawString(wchar_t*& dst, const wchar_t* src) {
	if (src == nullptr)
		return;

	const size_t len = std::min(std::wcslen(src), MAX_STRING_LENGTH-1);
	if (len == 0)
		return;

	std::wmemcpy(dst, src, len);
	dst += len;
}

void terminateRawString(wchar_t*& dst) {
	std::wmemset(dst, 0, 1);
	dst += 1;
}

void getHandleDataType(prt::Attributable::PrimitiveType prtType, adsk::Data::Member::eDataType& handleType,
					   uint32_t& itemCount, uint32_t& typeSize) {
	itemCount = 1;
	typeSize = 1;
	switch (prtType) {
		case prt::Attributable::PT_BOOL:
			handleType = adsk::Data::Member::kBoolean;
			break;
		case prt::Attributable::PT_FLOAT:
			handleType = adsk::Data::Member::kDouble;
			break;
		case prt::Attributable::PT_INT:
			handleType = adsk::Data::Member::kInt32;
			break;
		case prt::Attributable::PT_STRING:
			handleType = getWideCharacterType();
			typeSize = MAX_STRING_LENGTH; // 1 PRT string maps to HANDLE_MAX_STRING_LENGTH uint16/32 items
			break;
		case prt::Attributable::PT_BOOL_ARRAY:
			handleType = adsk::Data::Member::kBoolean;
			itemCount = MAX_ARRAY_SIZE;
			break;
		case prt::Attributable::PT_FLOAT_ARRAY:
			handleType = adsk::Data::Member::kDouble;
			itemCount = MAX_ARRAY_SIZE;
			break;
		case prt::Attributable::PT_INT_ARRAY:
			handleType = adsk::Data::Member::kInt32;
			itemCount = MAX_ARRAY_SIZE;
			break;
		case prt::Attributable::PT_STRING_ARRAY:
			handleType = getWideCharacterType();
			typeSize = MAX_STRING_LENGTH;
			itemCount = MAX_ARRAY_SIZE;
			break;
		default:
			handleType = adsk::Data::Member::kInvalidType;
			typeSize = 0;
			itemCount = 0;
			break;
	}
}

} // namespace

namespace mu {

int32_t computeSeed(const MFloatPoint& p) {
	float x = p[0];
	float z = p[2];
	int32_t seed = *(int32_t*)(&x);
	seed ^= *(int32_t*)(&z);
	seed %= 714025;
	return seed;
}

int32_t computeSeed(const MFloatPointArray& vertices) {
	MFloatPoint a(0.0, 0.0, 0.0);
	for (unsigned int vi = 0; vi < vertices.length(); vi++) {
		a += vertices[vi];
	}
	a = a / static_cast<float>(vertices.length());
	return computeSeed(a);
}

int32_t computeSeed(const double* vertices, size_t count) {
	MFloatPoint a(0.0, 0.0, 0.0);
	for (unsigned int vi = 0; vi < count; vi += 3) {
		a[0] += static_cast<float>(vertices[vi + 0]);
		a[1] += static_cast<float>(vertices[vi + 1]);
		a[2] += static_cast<float>(vertices[vi + 2]);
	}
	a = a / static_cast<float>(count);
	return computeSeed(a);
}

void statusCheck(const MStatus& status, const char* file, int line) {
	if (MS::kSuccess != status) {
		LOG_ERR << "maya status error at " << file << ":" << line << ": " << status.errorString().asChar() << " (code "
		        << status.statusCode() << ")";
	}
}

namespace structure {

adsk::Data::Structure* registerStructure(const std::string& name, const std::vector<Descriptor>& descs) {
	adsk::Data::Structure* structure = adsk::Data::Structure::structureByName(name.c_str());

	// ensure up-to-date structure
	if (structure != nullptr) {
		adsk::Data::Structure::deregisterStructure(*structure);
		// note: adsk::Data::Structure is ref-counted, we do not need to destroy it
	}

	structure = adsk::Data::Structure::create();
	structure->setName(name.c_str());

	for (const Descriptor& d : descs) {
		adsk::Data::Member::eDataType handleType = adsk::Data::Member::kInvalidType;
		uint32_t itemCount = 0;
		uint32_t typeSize = 0;
		getHandleDataType(d.mType, handleType, itemCount, typeSize);
		const uint32_t numberOfMayaDataItems = itemCount * typeSize;
		const std::string nKey = prtu::toOSNarrowFromUTF16(d.mKey);
		structure->addMember(handleType, numberOfMayaDataItems, nKey.c_str());
	}

	adsk::Data::Structure::registerStructure(*structure);
	return structure;
}

void putBool(adsk::Data::Handle& handle, const wchar_t* key, bool val) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == adsk::Data::Member::kBoolean);
	*handle.asBoolean() = val;
}

void putFloat(adsk::Data::Handle& handle, const wchar_t* key, double val) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == adsk::Data::Member::kDouble);
	*handle.asDouble() = val;
}

void putInt(adsk::Data::Handle& handle, const wchar_t* key, int32_t val) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == adsk::Data::Member::kInt32);
	*handle.asInt32() = val;
}

void putString(adsk::Data::Handle& handle, const wchar_t* key, const wchar_t* val) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == getWideCharacterType());
	wchar_t* destPtr = getWideCharacterPtr(handle);
	copyRawString(destPtr, val);
	terminateRawString(destPtr);
}

bool getBool(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);
	return *handle.asBoolean();
}

double getFloat(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);
	return *handle.asDouble();
}

int32_t getInt(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);
	return *handle.asInt32();
}

wchar_t const* getString(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);
	return getWideCharacterPtr(handle);
}

void putBoolArray(adsk::Data::Handle& handle, const wchar_t* key, const bool* vals, size_t count) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == adsk::Data::Member::kBoolean);
	std::copy(vals, vals + std::min(count, MAX_ARRAY_SIZE), handle.asBoolean());
}

void putFloatArray(adsk::Data::Handle& handle, const wchar_t* key, const double* vals, size_t count) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == adsk::Data::Member::kDouble);
	std::copy(vals, vals + std::min(count, MAX_ARRAY_SIZE), handle.asDouble());
}

void putIntArray(adsk::Data::Handle& handle, const wchar_t* key, const int32_t* vals, size_t count) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == adsk::Data::Member::kInt32);
	std::copy(vals, vals + std::min(count, MAX_ARRAY_SIZE), handle.asInt32());
}

void putStringArray(adsk::Data::Handle& handle, const wchar_t* key, wchar_t const* const* vals, size_t count) {
	setHandlePosition(handle, key);
	assert(handle.dataType() == getWideCharacterType());
	wchar_t* destPtr = getWideCharacterPtr(handle);

	// store the string array with fixed size, even if we pretend to support
	// arbitrary length arrays.
	for (size_t i = 0; i < MAX_ARRAY_SIZE; i++) {
		if (i < count)
			copyRawString(destPtr, vals[i]);
		terminateRawString(destPtr);
	}
}

std::pair<const bool*, size_t> getBoolArray(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);
	return {handle.asBoolean(), handle.dataLength()};
}

std::pair<const double*, size_t> getFloatArray(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);
	return {handle.asDouble(), handle.dataLength()};
}

std::pair<const int32_t*, size_t> getIntArray(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);
	return {handle.asInt32(), handle.dataLength()};
}

std::vector<const wchar_t*> getStringArray(adsk::Data::Handle& handle, const wchar_t* key) {
	setHandlePosition(handle, key);

	wchar_t* srcPtr = getWideCharacterPtr(handle);
	std::vector<const wchar_t*> result = { srcPtr };

	while (result.size() < MAX_ARRAY_SIZE) {
		while (*(++srcPtr) != 0);
		result.push_back(++srcPtr);
	}

	return result;
}

} // namespace structure

} // namespace mu