/*
 * MayaEncoder.h
 *
 *  Created on: Sep 11, 2012
 *      Author: shaegler
 */

#ifndef MAYAENCODER_H_
#define MAYAENCODER_H_

#include <string>
#include <iostream>
#include <stdexcept>

#include "prt/ContentType.h"
#include "prt/InitialShape.h"

#include "prtx/EncoderFactory.h"
#include "prtx/Encoder.h"

#include "encoder/IMayaOutputHandler.h"


class MayaEncoder : public prtx::IEncoder {
public:
	MayaEncoder();
	virtual ~MayaEncoder();

public:
	virtual void encode(prtx::IGenerateContext& context, size_t initialShapeIndex);

public:
//	virtual const wchar_t* getID() const { return ID.c_str(); }
	virtual prt::ContentType getContentType() const { return prt::CT_GEOMETRY; }

	static void destroyMayaData(struct MayaOutputHandler* mayaData);

public:
	static const std::wstring ID;
	static const std::wstring NAME;
	static const std::wstring DESCRIPTION;

private:
	void convertGeometry(prtx::AbstractResolveMapPtr am, prtx::GeometryPtrVector& geometries, IMayaOutputHandler* mayaOutput);
	void unpackRPK(std::wstring rpkPath);
};


class MayaEncoderFactory : public prtx::EncoderFactory {
public:
	MayaEncoderFactory() {
		setInfo(MayaEncoder::ID, MayaEncoder::NAME, MayaEncoder::DESCRIPTION);
		prt::AttributeMapBuilder* defaultOptions = prt::AttributeMapBuilder::create();
		setDefaultOptions(defaultOptions->createAttributeMap());
		defaultOptions->destroy();
	}

	virtual ~MayaEncoderFactory() {}

	MayaEncoder* create() { return new MayaEncoder(); }

	virtual const std::wstring& getID() const { return MayaEncoder::ID; }
	virtual const std::wstring& getName() const { return MayaEncoder::NAME; }
	virtual const std::wstring& getDescription() const { return MayaEncoder::DESCRIPTION; }
	virtual prt::ContentType getContentType() const { return prt::CT_GEOMETRY; }
};

#endif /* MAYAENCODER_H_ */
