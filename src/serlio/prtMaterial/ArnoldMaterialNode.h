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

#include "maya/MPxNode.h"

class MaterialInfo;
class MaterialTrafo;
class MELScriptBuilder;

class ArnoldMaterialNode : public MPxNode {

public:
	static MStatus initialize();

	static MTypeId id;
	static MObject aInMesh;
	static MObject aOutMesh;

	MStatus compute(const MPlug& plug, MDataBlock& data) override;

	MPxNode::SchedulingType schedulingType() const noexcept override {
		return SchedulingType::kGloballySerial;
	}

private:
	void buildMaterialShaderScript(MELScriptBuilder& sb,
								   const MaterialInfo& matInfo,
								   const std::wstring& shaderName,
								   const std::wstring& shadingGroupName,
								   const std::wstring& meshName,
								   const int faceStart,
								   const int faceEnd);

	void setUvTransformAttrs(MELScriptBuilder& sb,
							 const std::wstring& uvSet,
							 const MaterialTrafo& trafo);

	std::wstring createMapShader(MELScriptBuilder& sb,
								 const std::string& mapFile,
								 const MaterialTrafo& mapTrafo,
								 const std::wstring& shaderName,
								 const std::wstring& uvSet,
								 const bool raw,
								 const bool alpha);
};
