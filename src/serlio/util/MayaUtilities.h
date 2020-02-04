#pragma once

#include "util/LogHandler.h"

#include "maya/MFloatPointArray.h"
#include "maya/MFnAttribute.h"
#include "maya/MFnDependencyNode.h"
#include "maya/MObject.h"
#include "maya/MStatus.h"
#include "maya/MString.h"
#include "maya/adskDataHandle.h"

#include <utility>

#define MCHECK(status) mu::statusCheck((status), __FILE__, __LINE__);

// utility functions with dependencies on the Maya API
namespace mu {

int32_t computeSeed(const MFloatPointArray& vertices);
int32_t computeSeed(const double* vertices, size_t count);

void statusCheck(const MStatus& status, const char* file, int line);

template <typename F>
void forAllAttributes(const MFnDependencyNode& node, F func) {
	for (unsigned int i = 0; i < node.attributeCount(); i++) {
		const MObject attrObj = node.attribute(i);
		const MFnAttribute attr(attrObj);
		func(attr);
	}
}

namespace structure {

struct Descriptor {
	std::wstring mKey;
	prt::Attributable::PrimitiveType mType = prt::Attributable::PT_UNDEFINED;
};

adsk::Data::Structure* registerStructure(const std::string& name, const std::vector<Descriptor>& descs);

void putBool(adsk::Data::Handle& handle, const wchar_t* key, bool val);
void putFloat(adsk::Data::Handle& handle, const wchar_t* key, double val);
void putInt(adsk::Data::Handle& handle, const wchar_t* key, int32_t val);
void putString(adsk::Data::Handle& handle, const wchar_t* key, const wchar_t* val);

bool getBool(adsk::Data::Handle& handle, const wchar_t* key);
double getFloat(adsk::Data::Handle& handle, const wchar_t* key);
int32_t getInt(adsk::Data::Handle& handle, const wchar_t* key);
wchar_t const* getString(adsk::Data::Handle& handle, const wchar_t* key);

void putBoolArray(adsk::Data::Handle& handle, const wchar_t* key, const bool* vals, size_t count);
void putFloatArray(adsk::Data::Handle& handle, const wchar_t* key, const double* vals, size_t count);
void putIntArray(adsk::Data::Handle& handle, const wchar_t* key, const int32_t* vals, size_t count);
void putStringArray(adsk::Data::Handle& handle, const wchar_t* key, wchar_t const* const* vals, size_t count);

std::pair<const bool*, size_t> getBoolArray(adsk::Data::Handle& handle, const wchar_t* key);
std::pair<const double*, size_t> getFloatArray(adsk::Data::Handle& handle, const wchar_t* key);
std::pair<const int32_t*, size_t> getIntArray(adsk::Data::Handle& handle, const wchar_t* key);
std::vector<const wchar_t*> getStringArray(adsk::Data::Handle& handle, const wchar_t* key);

} // namespace structure

} // namespace mu