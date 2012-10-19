/*
 * MayaEncoder.cpp
 *
 *  Created on: Sep 11, 2012
 *      Author: shaegler
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <numeric>

#define BOOST_FILESYSTEM_VERSION 3
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include "api/prtapi.h"

#include "spi/base/SPIException.h"
#include "spi/base/Log.h"
#include "spi/base/IGeometry.h"
#include "spi/base/IShape.h"
#include "spi/base/ILeafIterator.h"
#include "spi/codec/EncodePreparator.h"
#include "spi/extension/ExtensionManager.h"

#include "util/StringUtils.h"
#include "util/Timer.h"
#include "util/URIUtils.h"
#include "util/Exception.h"

#include "encoder/MayaEncoder.h"


MayaEncoder::MayaEncoder() {
}


MayaEncoder::~MayaEncoder() {
}


void MayaEncoder::encode(const prt::InitialShape** initialShapes, size_t initialShapeCount, prtspi::AbstractResolveMapPtr am, const prt::AttributeMap* options, prt::OutputHandler* const outputHandler) {
	am = am->toFileURIs();

	IMayaOutputHandler* oh = dynamic_cast<IMayaOutputHandler*>(outputHandler);
	if(oh == 0) throw(prtspi::StatusException(prt::STATUS_ILLEGAL_OUTPUT_HANDLER));

	Timer tim;
	log_trace("MayaEncoder:encode: #initial shapes = %d", initialShapeCount);

	prtspi::EncodePreparator* encPrep = prtspi::EncodePreparator::create();
	for (size_t i = 0; i < initialShapeCount; ++i) {
		prtspi::IGeometry** occluders = 0;
		prtspi::ILeafIterator* li = prtspi::ILeafIterator::create(initialShapes[i], am, occluders, 0);
		for (const prtspi::IShape* shape = li->getNext(); shape != 0; shape = li->getNext()) {
			encPrep->add(/*initialShapes[i],*/ shape);
			//			log_trace(L"encode leaf shape mat: %ls", shape->getMaterial()->getString(L"name"));
		}
	}

	const float t1 = tim.stop();
	tim.start();

	prtspi::IContentArray* geometries = prtspi::IContentArray::create();
	encPrep->createEncodableGeometries(geometries);
	convertGeometry(am, geometries, oh);
	geometries->destroy();

	encPrep->destroy();

	const float t2 = tim.stop();
	log_info("MayaEncoder::encode() : preparator %f s, encoding %f s, total %f s", t1, t2, t1+t2);

	log_trace("MayaEncoder::encode done.");
}


void MayaEncoder::convertGeometry(prtspi::AbstractResolveMapPtr am, prtspi::IContentArray* geometries, IMayaOutputHandler* mdata) {
	std::vector<double> vertices;
	std::vector<int> counts;
	std::vector<int> connects;

	std::vector<float> tcsU, tcsV;
	std::vector<int> uvCounts;
	std::vector<int> uvConnects;

	uint32_t base = 0;
	uint32_t tcBase = 0;
	for(size_t gi = 0, size = geometries->size(); gi < size; ++gi) {
		prtspi::IGeometry* geo = (prtspi::IGeometry*)geometries->get(gi);

		const double* verts = geo->getVertices();
		const size_t vertsCount = geo->getVertexCount();

		for(size_t i = 0; i < vertsCount*3; ++i)
			vertices.push_back(verts[i]);

		const size_t tcsCount = geo->getUVCount();
		if(tcsCount > 0) {
			const double* tcs = geo->getUVs();
			for(size_t i=0; i<tcsCount; i++) {
				tcsU.push_back((float)tcs[i*2]);
				tcsV.push_back((float)tcs[i*2+1]);
			}
		}

		for(size_t fi = 0; fi < geo->getFaceCount(); ++fi) {
			const prtspi::IFace* face = geo->getFace(fi);
			counts.push_back(face->getIndexCount());

			const uint32_t* indices = face->getVertexIndices();
			for(size_t vi = 0; vi < face->getIndexCount(); ++vi)
				connects.push_back(base + indices[vi]);

			uvCounts.push_back(face->getUVIndexCount());
			for(size_t vi = 0; vi < face->getUVIndexCount(); ++vi)
				uvConnects.push_back(tcBase + face->getUVIndices()[vi]);
		}

		base   = vertices.size() / 3;
		tcBase = tcsU.size();
	}

//	std::cout << "uvCounts: " << uvCounts << std::endl;
//	std::cout << "uvConnects: " << uvConnects << std::endl;
//	std::cout << "uvCounts sum:" << std::accumulate(uvCounts.begin(), uvCounts.end(), 0) << std::endl;
//	std::cout << "tcsU size = " << tcsU.size() << std::endl;

	mdata->setVertices(&vertices[0], vertices.size());
	mdata->setUVs(&tcsU[0], &tcsV[0], tcsU.size());
	mdata->setFaces(&counts[0], counts.size(), &connects[0], connects.size(), &uvCounts[0], uvCounts.size(), &uvConnects[0], uvConnects.size());
	mdata->createMesh();

#if 0
	int startFace = 0;
	for(size_t gi = 0, size = geometries->size(); gi < size; ++gi) {
		prtspi::IGeometry* geo = (prtspi::IGeometry*)geometries->get(gi);
		prtspi::IMaterial* mat = geo->getMaterial();
		const int faceCount   = (int)geo->getFaceCount();

		std::wcout << L"creating material: '" << mat->getString(L"name") << L"'" << std::endl;

		int mh = mdata->matCreate(mat->getString(L"name").c_str(), startFace, faceCount);

		std::wstring tex;
		if(mat->getTextureArray(L"diffuseMap")->size() > 0) {
			std::wstring uri(mat->getTextureArray(L"diffuseMap")->get(0)->getName());
			tex = uri.substr(wcslen(URIUtils::SCHEME_FILE));
			mdata->matSetDiffuseTexture(mh, tex.c_str());
		}

		startFace += faceCount;
	}
#endif

	mdata->finishMesh();
}


void MayaEncoder::unpackRPK(std::wstring rpkPath) {

}

