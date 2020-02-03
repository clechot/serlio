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

#include "maya/MString.h"
#include "maya/adskDataHandle.h"

#include <array>

const std::string gPRTMatStructure = "prtMaterialStructure";
const std::string gPRTMatChannel = "prtMaterialChannel";
const std::string gPRTMatStream = "prtMaterialStream";
const std::wstring gPRTMatMemberFaceStart = L"faceIndexStart";
const std::wstring gPRTMatMemberFaceEnd = L"faceIndexEnd";

class MaterialColor {
public:
	MaterialColor(adsk::Data::Handle& handle, const std::wstring& name);

	double r() const noexcept;
	double g() const noexcept;
	double b() const noexcept;

	bool operator==(const MaterialColor& other) const noexcept;
	bool operator<(const MaterialColor& rhs) const noexcept;
	bool operator>(const MaterialColor& rhs) const noexcept;

private:
	std::array<double, 3> data;
};

class MaterialTrafo {
public:
	MaterialTrafo(adsk::Data::Handle& handle, const std::wstring& name);

	double su() const noexcept;
	double sv() const noexcept;
	double tu() const noexcept;
	double tv() const noexcept;
	double rw() const noexcept;

	std::array<double, 2> tuv() const noexcept;
	std::array<double, 3> suvw() const noexcept;

	bool operator==(const MaterialTrafo& other) const noexcept;
	bool operator<(const MaterialTrafo& rhs) const noexcept;
	bool operator>(const MaterialTrafo& rhs) const noexcept;

private:
	std::array<double, 5> data;
};

class MaterialInfo {
public:
	explicit MaterialInfo(adsk::Data::Handle& handle);

	std::wstring bumpMap;
	std::wstring colormap;
	std::wstring dirtmap;
	std::wstring emissiveMap;
	std::wstring metallicMap;
	std::wstring normalMap;
	std::wstring occlusionMap;
	std::wstring opacityMap;
	std::wstring roughnessMap;
	std::wstring specularMap;

	double opacity = 1.0;
	double metallic = 0.0;
	double roughness = 1.0;

	MaterialColor ambientColor;
	MaterialColor diffuseColor;
	MaterialColor emissiveColor;
	MaterialColor specularColor;

	MaterialTrafo specularmapTrafo;
	MaterialTrafo bumpmapTrafo;
	MaterialTrafo colormapTrafo;
	MaterialTrafo dirtmapTrafo;
	MaterialTrafo emissivemapTrafo;
	MaterialTrafo metallicmapTrafo;
	MaterialTrafo normalmapTrafo;
	MaterialTrafo occlusionmapTrafo;
	MaterialTrafo opacitymapTrafo;
	MaterialTrafo roughnessmapTrafo;

	bool equals(const MaterialInfo& o) const;

	bool operator<(const MaterialInfo& rhs) const;
};
