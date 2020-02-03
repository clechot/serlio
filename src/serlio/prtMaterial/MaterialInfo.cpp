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

#include "prtMaterial/MaterialInfo.h"
#include "prtMaterial/MaterialUtils.h"

#include "util/MayaUtilities.h"

#include <cmath>

namespace {

template <size_t N>
void getDoubleArray(std::array<double, N>& array, adsk::Data::Handle& handle, const std::wstring& name) {
	const std::pair<const double*, size_t> result = mu::structure::getFloatArray(handle, name.c_str());
	std::copy(result.first, result.first + result.second, array.begin());
}

std::wstring getTexture(adsk::Data::Handle& handle, const std::wstring& texName) {
	wchar_t const* const strPtr = mu::structure::getString(handle, texName.c_str());
	return (strPtr != nullptr) ? strPtr : std::wstring();
}

double getDouble(adsk::Data::Handle& handle, const std::wstring& name) {
	return mu::structure::getFloat(handle, name.c_str());
}

} // namespace

MaterialColor::MaterialColor(adsk::Data::Handle& handle, const std::wstring& name) {
	getDoubleArray(data, handle, name);
}

double MaterialColor::r() const noexcept {
	return data[0];
}

double MaterialColor::g() const noexcept {
	return data[1];
}

double MaterialColor::b() const noexcept {
	return data[2];
}

bool MaterialColor::operator==(const MaterialColor& other) const noexcept {
	return this->data == other.data;
}

bool MaterialColor::operator<(const MaterialColor& rhs) const noexcept {
	return this->data < rhs.data;
}

bool MaterialColor::operator>(const MaterialColor& rhs) const noexcept {
	return rhs < *this;
}

MaterialTrafo::MaterialTrafo(adsk::Data::Handle& handle, const std::wstring& name) {
	getDoubleArray(data, handle, name);
}

double MaterialTrafo::su() const noexcept {
	return data[0];
}

double MaterialTrafo::sv() const noexcept {
	return data[1];
}

double MaterialTrafo::tu() const noexcept {
	return data[2];
}

double MaterialTrafo::tv() const noexcept {
	return data[3];
}

double MaterialTrafo::rw() const noexcept {
	return data[4];
}

std::array<double, 2> MaterialTrafo::tuv() const noexcept {
	return {tu(), tv()};
}

std::array<double, 3> MaterialTrafo::suvw() const noexcept {
	return {su(), sv(), rw()};
}

bool MaterialTrafo::operator==(const MaterialTrafo& other) const noexcept {
	return this->data == other.data;
}

bool MaterialTrafo::operator<(const MaterialTrafo& rhs) const noexcept {
	return this->data < rhs.data;
}

bool MaterialTrafo::operator>(const MaterialTrafo& rhs) const noexcept {
	return rhs < *this;
}

MaterialInfo::MaterialInfo(adsk::Data::Handle& handle)
    : bumpMap(getTexture(handle, L"bumpMap")), colormap(getTexture(handle, L"diffuseMap")),
      dirtmap(getTexture(handle, L"diffuseMap1")), emissiveMap(getTexture(handle, L"emissiveMap")),
      metallicMap(getTexture(handle, L"metallicMap")), normalMap(getTexture(handle, L"normalMap")),
      occlusionMap(getTexture(handle, L"occlusionMap")), opacityMap(getTexture(handle, L"opacityMap")),
      roughnessMap(getTexture(handle, L"roughnessMap")), specularMap(getTexture(handle, L"specularMap")),

      opacity(getDouble(handle, L"opacity")), metallic(getDouble(handle, L"metallic")),
      roughness(getDouble(handle, L"roughness")),

      ambientColor(handle, L"ambientColor"), bumpmapTrafo(handle, L"bumpmapTrafo"),
      colormapTrafo(handle, L"colormapTrafo"), diffuseColor(handle, L"diffuseColor"),
      dirtmapTrafo(handle, L"dirtmapTrafo"), emissiveColor(handle, L"emissiveColor"),
      emissivemapTrafo(handle, L"emissivemapTrafo"), metallicmapTrafo(handle, L"metallicmapTrafo"),
      normalmapTrafo(handle, L"normalmapTrafo"), occlusionmapTrafo(handle, L"occlusionmapTrafo"),
      opacitymapTrafo(handle, L"opacitymapTrafo"), roughnessmapTrafo(handle, L"roughnessmapTrafo"),
      specularColor(handle, L"specularColor"), specularmapTrafo(handle, L"specularmapTrafo") {}

bool MaterialInfo::equals(const MaterialInfo& o) const {
	// clang-format off
	return
	        bumpMap == o.bumpMap &&
	        colormap == o.colormap &&
	        dirtmap == o.dirtmap &&
	        emissiveMap == o.emissiveMap &&
	        metallicMap == o.metallicMap &&
	        normalMap == o.normalMap &&
	        occlusionMap == o.occlusionMap &&
	        opacityMap == o.opacityMap &&
	        roughnessMap == o.roughnessMap &&
	        specularMap == o.specularMap &&
	        opacity == o.opacity &&
	        metallic == o.metallic &&
	        roughness == o.roughness &&
	        ambientColor == o.ambientColor &&
	        bumpmapTrafo == o.bumpmapTrafo &&
	        colormapTrafo == o.colormapTrafo &&
	        diffuseColor == o.diffuseColor &&
	        dirtmapTrafo == o.dirtmapTrafo &&
	        emissiveColor == o.emissiveColor &&
	        emissivemapTrafo == o.emissivemapTrafo &&
	        metallicmapTrafo == o.metallicmapTrafo &&
	        normalmapTrafo == o.normalmapTrafo &&
	        occlusionmapTrafo == o.occlusionmapTrafo &&
	        opacitymapTrafo == o.opacitymapTrafo &&
	        roughnessmapTrafo == o.roughnessmapTrafo &&
	        specularColor == o.specularColor &&
	        specularmapTrafo == o.specularmapTrafo;
	// clang-format on
}

bool MaterialInfo::operator<(const MaterialInfo& rhs) const {
	// clang-format off
	{ int c = bumpMap.compare(rhs.bumpMap);           if (c != 0) return (c < 0); }
	{ int c = colormap.compare(rhs.colormap);         if (c != 0) return (c < 0); }
	{ int c = dirtmap.compare(rhs.dirtmap);           if (c != 0) return (c < 0); }
	{ int c = emissiveMap.compare(rhs.emissiveMap);   if (c != 0) return (c < 0); }
	{ int c = metallicMap.compare(rhs.metallicMap);   if (c != 0) return (c < 0); }
	{ int c = normalMap.compare(rhs.normalMap);       if (c != 0) return (c < 0); }
	{ int c = occlusionMap.compare(rhs.occlusionMap); if (c != 0) return (c < 0); }
	{ int c = opacityMap.compare(rhs.opacityMap);     if (c != 0) return (c < 0); }
	{ int c = roughnessMap.compare(rhs.roughnessMap); if (c != 0) return (c < 0); }
	{ int c = specularMap.compare(rhs.specularMap);   if (c != 0) return (c < 0); }

	if (opacity > rhs.opacity) return false;
	if (opacity < rhs.opacity) return true;

	if (metallic > rhs.metallic) return false;
	if (metallic < rhs.metallic) return true;

	if (roughness > rhs.roughness) return false;
	if (roughness < rhs.roughness) return true;

	if (ambientColor > rhs.ambientColor) return false;
	if (ambientColor < rhs.ambientColor) return true;

	if (diffuseColor > rhs.diffuseColor) return false;
	if (diffuseColor < rhs.diffuseColor) return true;

	if (emissiveColor > rhs.emissiveColor) return false;
	if (emissiveColor < rhs.emissiveColor) return true;

	if (specularColor > rhs.specularColor) return false;
	if (specularColor < rhs.specularColor) return true;

	if (specularmapTrafo > rhs.specularmapTrafo) return false;
	if (specularmapTrafo < rhs.specularmapTrafo) return true;

	if (bumpmapTrafo > rhs.bumpmapTrafo) return false;
	if (bumpmapTrafo < rhs.bumpmapTrafo) return true;

	if (dirtmapTrafo > rhs.dirtmapTrafo) return false;
	if (dirtmapTrafo < rhs.dirtmapTrafo) return true;

	if (emissivemapTrafo > rhs.emissivemapTrafo) return false;
	if (emissivemapTrafo < rhs.emissivemapTrafo) return true;

	if (metallicmapTrafo > rhs.metallicmapTrafo) return false;
	if (metallicmapTrafo < rhs.metallicmapTrafo) return true;

	if (normalmapTrafo > rhs.normalmapTrafo) return false;
	if (normalmapTrafo < rhs.normalmapTrafo) return true;

	if (occlusionmapTrafo > rhs.occlusionmapTrafo) return false;
	if (occlusionmapTrafo < rhs.occlusionmapTrafo) return true;

	if (opacitymapTrafo > rhs.opacitymapTrafo) return false;
	if (opacitymapTrafo < rhs.opacitymapTrafo) return true;

	if (roughnessmapTrafo > rhs.roughnessmapTrafo) return false;
	if (roughnessmapTrafo < rhs.roughnessmapTrafo) return true;

	if (specularmapTrafo > rhs.specularmapTrafo) return false;
	if (specularmapTrafo < rhs.specularmapTrafo) return true;
	// clang-format on

	return false; // equality
}
