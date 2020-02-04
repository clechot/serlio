/**
 * Serlio - Esri CityEngine Plugin for Autodesk Maya
 *
 * See https://github.com/esri/serlio for build and usage instructions.
 *
 * Copyright (c) 2012-2019 Esri R&D Center Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "prtModifier/MayaCallbacks.h"
#include "prtModifier/PRTModifierNode.h"

#include "prtMaterial/MaterialInfo.h"
#include "prtMaterial/MaterialUtils.h"

#include "util/LogHandler.h"
#include "util/MayaUtilities.h"
#include "util/Utilities.h"

#include "prt/StringUtils.h"

#include "maya/MFloatArray.h"
#include "maya/MFloatPointArray.h"
#include "maya/MFloatVectorArray.h"
#include "maya/MFnMesh.h"
#include "maya/MFnMeshData.h"
#include "maya/adskDataAssociations.h"
#include "maya/adskDataStream.h"

#include <cassert>
#include <sstream>

namespace {

constexpr bool DBG = false;

MIntArray toMayaIntArray(uint32_t const* a, size_t s) {
	MIntArray mia(static_cast<unsigned int>(s), 0);
	for (unsigned int i = 0; i < s; ++i)
		mia.set(a[i], i);
	return mia;
}

MFloatPointArray toMayaFloatPointArray(double const* a, size_t s) {
	assert(s % 3 == 0);
	const unsigned int numPoints = static_cast<unsigned int>(s) / 3;
	MFloatPointArray mfpa(numPoints);
	for (unsigned int i = 0; i < numPoints; ++i) {
		mfpa.set(MFloatPoint(static_cast<float>(a[i * 3 + 0]), static_cast<float>(a[i * 3 + 1]),
		                     static_cast<float>(a[i * 3 + 2])),
		         i);
	}
	return mfpa;
}

} // namespace

struct TextureUVOrder {
	MString mayaUvSetName;
	uint8_t mayaUvSetIndex;
	uint8_t prtUvSetIndex;
};

// maya pbr stingray shader only supports first 4 uvsets -> reoder so first 4 are most important ones
// other shaders support >4 sets
const std::vector<TextureUVOrder> TEXTURE_UV_ORDERS = []() -> std::vector<TextureUVOrder> {
	// clang-format off
	return {
	        // maya uvset name | maya idx | prt idx  | CGA key
	        { L"map1",         0,    0 },  // colormap
	        { L"dirtMap",      1,    2 },  // dirtmap
	        { L"normalMap",    2,    5 },  // normalmap
	        { L"opacityMap",   3,    4 },  // opacitymap

	        { L"bumpMap",      4,    1 },  // bumpmap
	        { L"specularMap",  5,    3 },  // specularmap
	        { L"emissiveMap",  6,    6 },  // emissivemap
	        { L"occlusionMap", 7,    7 },  // occlusionmap
	        { L"roughnessMap", 8,    8 },  // roughnessmap
	        { L"metallicMap",  9,    9 }   // metallicmap
	};
	// clang-format on
}();

void MayaCallbacks::addMesh(const wchar_t*, const double* vtx, size_t vtxSize, const double* nrm, size_t nrmSize,
                            const uint32_t* faceCounts, size_t faceCountsSize, const uint32_t* vertexIndices,
                            size_t vertexIndicesSize, const uint32_t* normalIndices,
                            MAYBE_UNUSED size_t normalIndicesSize, double const* const* uvs, size_t const* uvsSizes,
                            uint32_t const* const* uvCounts, size_t const* uvCountsSizes,
                            uint32_t const* const* uvIndices, size_t const* uvIndicesSizes, size_t uvSetsCount,
                            const uint32_t* faceRanges, size_t faceRangesSize, const prt::AttributeMap** materials,
                            const prt::AttributeMap** /*reports*/, const int32_t*) {
	MFloatPointArray mayaVertices = toMayaFloatPointArray(vtx, vtxSize);
	MIntArray mayaFaceCounts = toMayaIntArray(faceCounts, faceCountsSize);
	MIntArray mayaVertexIndices = toMayaIntArray(vertexIndices, vertexIndicesSize);

	if (DBG) {
		LOG_DBG << "-- MayaCallbacks::addMesh";
		LOG_DBG << "   faceCountsSize = " << faceCountsSize;
		LOG_DBG << "   vertexIndicesSize = " << vertexIndicesSize;
		LOG_DBG << "   mayaVertices.length         = " << mayaVertices.length();
		LOG_DBG << "   mayaFaceCounts.length   = " << mayaFaceCounts.length();
		LOG_DBG << "   mayaVertexIndices.length = " << mayaVertexIndices.length();
	}

	MStatus stat;
	MCHECK(stat);

	MFnMeshData dataCreator;
	MObject newOutputData = dataCreator.create(&stat);
	MCHECK(stat);

	MFnMesh mFnMesh1;
	MObject oMesh = mFnMesh1.create(mayaVertices.length(), mayaFaceCounts.length(), mayaVertices, mayaFaceCounts,
	                                mayaVertexIndices, newOutputData, &stat);
	MCHECK(stat);

	MFnMesh mFnMesh(oMesh);
	mFnMesh.clearUVs();

	// -- add texture coordinates
	for (const TextureUVOrder& o : TEXTURE_UV_ORDERS) {
		uint8_t uvSet = o.prtUvSetIndex;

		if (uvSetsCount > uvSet && uvsSizes[uvSet] > 0) {

			MFloatArray mU;
			MFloatArray mV;
			for (size_t uvIdx = 0; uvIdx < uvsSizes[uvSet] / 2; ++uvIdx) {
				mU.append(static_cast<float>(uvs[uvSet][uvIdx * 2 + 0])); // maya mesh only supports float uvs
				mV.append(static_cast<float>(uvs[uvSet][uvIdx * 2 + 1]));
			}

			MString uvSetName = o.mayaUvSetName;

			if (uvSet != 0) {
				mFnMesh.createUVSetDataMeshWithName(uvSetName, &stat);
				MCHECK(stat);
			}

			MCHECK(mFnMesh.setUVs(mU, mV, &uvSetName));

			MIntArray mUVCounts = toMayaIntArray(uvCounts[uvSet], uvCountsSizes[uvSet]);
			MIntArray mUVIndices = toMayaIntArray(uvIndices[uvSet], uvIndicesSizes[uvSet]);
			MCHECK(mFnMesh.assignUVs(mUVCounts, mUVIndices, &uvSetName));
		}
		else {
			if (uvSet > 0) {
				// add empty set to keep order consistent
				mFnMesh.createUVSetDataMeshWithName(o.mayaUvSetName, &stat);
				MCHECK(stat);
			}
		}
	}

	if (nrmSize > 0) {
		assert(normalIndicesSize == vertexIndicesSize);
		// guaranteed by MayaEncoder, see prtx::VertexNormalProcessor::SET_MISSING_TO_FACE_NORMALS

		// convert to native maya normal layout
		MVectorArray expandedNormals(static_cast<unsigned int>(vertexIndicesSize));
		MIntArray faceList(static_cast<unsigned int>(vertexIndicesSize));

		int indexCount = 0;
		for (int i = 0; i < faceCountsSize; i++) {
			int faceLength = mayaFaceCounts[i];

			for (int j = 0; j < faceLength; j++) {
				faceList[indexCount] = i;
				int idx = normalIndices[indexCount];
				expandedNormals.set(&nrm[idx * 3], indexCount);
				indexCount++;
			}
		}

		MCHECK(mFnMesh.setFaceVertexNormals(expandedNormals, faceList, mayaVertexIndices));
	}

	MFnMesh outputMesh(outMeshObj);
	outputMesh.copyInPlace(oMesh);

	// create material metadata
	adsk::Data::Structure* materialStructure = nullptr;
	if ((materials != nullptr) && (faceRangesSize > 1)) {
		const prt::AttributeMap* mat = materials[0];

		// TODO: structure for material should be setup statically
		std::vector<mu::structure::Descriptor> structureDescriptors = {
		        {gPRTMatMemberFaceStart, prt::Attributable::PT_INT},
		        {gPRTMatMemberFaceEnd, prt::Attributable::PT_INT}};

		size_t keyCount = 0;
		wchar_t const* const* keys = mat->getKeys(&keyCount);
		for (size_t k = 0; k < keyCount; k++) {
			wchar_t const* key = keys[k];
			structureDescriptors.push_back({key, mat->getType(key)});
		}

		materialStructure = mu::structure::registerStructure(gPRTMatStructure, structureDescriptors);
	}

	MCHECK(stat);
	MFnMesh inputMesh(inMeshObj);

	adsk::Data::Associations newMetadata(inputMesh.metadata(&stat));
	newMetadata.makeUnique();
	MCHECK(stat);
	adsk::Data::Channel newChannel = newMetadata.channel(gPRTMatChannel);
	adsk::Data::Stream newStream(*materialStructure, gPRTMatStream);

	newChannel.setDataStream(newStream);
	newMetadata.setChannel(newChannel);

	if (faceRangesSize > 1) {

		for (size_t fri = 0; fri < faceRangesSize - 1; fri++) {

			if (materials != nullptr) {
				adsk::Data::Handle handle(*materialStructure);

				const prt::AttributeMap* mat = materials[fri];

				size_t keyCount = 0;
				wchar_t const* const* keys = mat->getKeys(&keyCount);
				for (int k = 0; k < keyCount; k++) {
					wchar_t const* key = keys[k];

					switch (mat->getType(key)) {
						case prt::Attributable::PT_BOOL:
							mu::structure::putBool(handle, key, mat->getBool(key));
							break;
						case prt::Attributable::PT_FLOAT:
							mu::structure::putFloat(handle, key, mat->getFloat(key));
							break;
						case prt::Attributable::PT_INT:
							mu::structure::putInt(handle, key, mat->getInt(key));
							break;
						case prt::Attributable::PT_STRING:
							mu::structure::putString(handle, key, mat->getString(key));
							break;
						case prt::Attributable::PT_BOOL_ARRAY: {
							size_t count = 0;
							const bool* vals = mat->getBoolArray(key, &count);
							mu::structure::putBoolArray(handle, key, vals, count);
							break;
						}
						case prt::Attributable::PT_FLOAT_ARRAY: {
							size_t count = 0;
							const double* vals = mat->getFloatArray(key, &count);
							mu::structure::putFloatArray(handle, key, vals, count);
							break;
						}
						case prt::Attributable::PT_INT_ARRAY: {
							size_t count = 0;
							const int32_t* vals = mat->getIntArray(key, &count);
							mu::structure::putIntArray(handle, key, vals, count);
							break;
						}
						case prt::Attributable::PT_STRING_ARRAY: {
							size_t count = 0;
							wchar_t const* const* const vals = mat->getStringArray(key, &count);
							mu::structure::putStringArray(handle, key, vals, count);
							break;
						}
						default:
							break;
					}
				}

				mu::structure::putInt(handle, gPRTMatMemberFaceStart.c_str(), faceRanges[fri]);
				mu::structure::putInt(handle, gPRTMatMemberFaceEnd.c_str(), faceRanges[fri + 1]);

				newStream.setElement(static_cast<adsk::Data::IndexCount>(fri), handle);
			}
		}
	}

	outputMesh.setMetadata(newMetadata);
}

prt::Status MayaCallbacks::attrBool(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* key, bool value) {
	mAttributeMapBuilder->setBool(key, value);
	return prt::STATUS_OK;
}

prt::Status MayaCallbacks::attrFloat(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* key, double value) {
	mAttributeMapBuilder->setFloat(key, value);
	return prt::STATUS_OK;
}

prt::Status MayaCallbacks::attrString(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* key,
                                      const wchar_t* value) {
	mAttributeMapBuilder->setString(key, value);
	return prt::STATUS_OK;
}

// PRT version >= 2.1
#if PRT_VERSION_GTE(2, 1)

prt::Status MayaCallbacks::attrBoolArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* key,
                                         const bool* values, size_t size) {
	mAttributeMapBuilder->setBoolArray(key, values, size);
	return prt::STATUS_OK;
}

prt::Status MayaCallbacks::attrFloatArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* key,
                                          const double* values, size_t size) {
	mAttributeMapBuilder->setFloatArray(key, values, size);
	return prt::STATUS_OK;
}

prt::Status MayaCallbacks::attrStringArray(size_t /*isIndex*/, int32_t /*shapeID*/, const wchar_t* key,
                                           const wchar_t* const* values, size_t size) {
	mAttributeMapBuilder->setStringArray(key, values, size);
	return prt::STATUS_OK;
}

#endif // PRT version >= 2.1
