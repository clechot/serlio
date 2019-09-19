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

#pragma once

#include <maya/MString.h>
#include <maya/adskDataHandle.h>

#include <vector>

const std::string gPRTMatStructure = "prtMaterialStructure";
const std::string gPRTMatChannel = "prtMaterialChannel";
const std::string gPRTMatStream = "prtMaterialStream";
const std::string gPRTMatMemberFaceStart = "faceIndexStart";
const std::string gPRTMatMemberFaceEnd = "faceIndexEnd";

class MaterialInfo {
public:
	explicit MaterialInfo(adsk::Data::Handle sHandle);

	std::string bumpMap;
	std::string colormap;
	std::string dirtmap;
	std::string emissiveMap;
	std::string metallicMap;
	std::string normalMap;
	std::string occlusionMap;
	std::string opacityMap;
	std::string roughnessMap;
	std::string specularMap;

	double opacity;
	double metallic;
	double roughness;

	std::vector<double> ambientColor;
	std::vector<double> diffuseColor;
	std::vector<double> emissiveColor;
	std::vector<double> specularColor;

	std::vector<double> specularmapTrafo;
	std::vector<double> bumpmapTrafo;
	std::vector<double> colormapTrafo;
	std::vector<double> dirtmapTrafo;
	std::vector<double> emissivemapTrafo;
	std::vector<double> metallicmapTrafo;
	std::vector<double> normalmapTrafo;
	std::vector<double> occlusionmapTrafo;
	std::vector<double> opacitymapTrafo;
	std::vector<double> roughnessmapTrafo;

	bool equals(const MaterialInfo& o) const;
	static MString toMString(const std::vector<double>& d, size_t size, size_t offset);

private:
	static std::string getTexture(adsk::Data::Handle sHandle, const std::string& texName);
	static std::vector<double> getDoubleVector(adsk::Data::Handle sHandle, const std::string& name, size_t numElements);
	static double getDouble(adsk::Data::Handle sHandle, const std::string& name);
};
