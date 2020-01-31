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

#include "maya/MGlobal.h"

#include <array>
#include <sstream>
#include <string>

class MaterialColor;

class MELScriptBuilder {

public:
	void setAttr(const std::wstring& attribute, bool val);

	void setAttr(const std::wstring& attribute, int val);

	void setAttr(const std::wstring& attribute, double val);

	void setAttr(const std::wstring& attribute, double val1, double val2);
	void setAttr(const std::wstring& attribute, const std::array<double, 2>& val);

	void setAttr(const std::wstring& attribute, double val1, double val2, double val3);
	void setAttr(const std::wstring& attribute, const std::array<double, 3>& val);

	void setAttr(const std::wstring& attribute, const wchar_t* val);

	void setAttr(const std::wstring& attribute, const std::wstring& val);

	void setAttr(const std::wstring& attribute, const MaterialColor& color);

	void connectAttr(const std::wstring& source, const std::wstring& dest);

	void python(const std::wstring& pythonCmd);

	void declInt(const std::wstring& varName);
	void declString(const std::wstring& varName);

	void setVar(const std::wstring& varName, const std::wstring& val);

	void setsCreate(const std::wstring& setName);

	void setsAddFaceRange(const std::wstring& setName, const std::wstring& meshName, int faceStart, int faceEnd);

	void createShader(const std::wstring& shaderType, const std::wstring& shaderName);

	void createTexture(const std::wstring& textureName);

	void addCmdLine(const std::wstring& line);

	MStatus executeSync(std::wstring& output);

	MStatus execute();

private:
	std::wstringstream commandStream;
};
