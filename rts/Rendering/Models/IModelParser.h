/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef IMODELPARSER_H
#define IMODELPARSER_H

#include <unordered_map>
#include <string>
#include <deque>

#include "3DModel.h"
#include "System/Matrix44f.h"
#include "System/Threading/SpringMutex.h"

namespace boost {
	class thread;
};


class IModelParser
{
public:
	virtual ~IModelParser() {}
	virtual S3DModel* Load(const std::string& name) = 0;
	virtual ModelType GetType() const = 0;
};


struct LoadQueue {
public:
	LoadQueue(): thread(nullptr) {}
	~LoadQueue();

	void Pump();
	void Push(const std::string& modelName);

	void GrabLock() { mutex.lock(); }
	void FreeLock() { mutex.unlock(); }

private:
	std::deque<std::string> queue;
	spring::mutex mutex;
	boost::thread* thread;
};


class C3DModelLoader
{
public:
	C3DModelLoader();
	~C3DModelLoader();

	std::string FindModelPath(std::string name) const;

	S3DModel* LoadCached3DModel(const std::string& cachedModelName, bool preload);
	S3DModel* Load3DModel(std::string modelName, bool preload = false);

	void Preload3DModel(const std::string& modelName) { loadQueue.Push(modelName); }

	typedef std::unordered_map<std::string, unsigned int> ModelMap; // "armflash.3do" --> id
	typedef std::unordered_map<std::string, unsigned int> FormatMap; // "3do" --> MODELTYPE_3DO
	typedef std::unordered_map<unsigned int, IModelParser*> ParserMap; // MODELTYPE_3DO --> parser

private:
	void AddModelToCache(S3DModel* model, const std::string& modelName, const std::string& modelPath);
	void CreateLists(S3DModel* o);
	void CreateListsNow(S3DModelPiece* o);

	ModelMap cache;
	FormatMap formats;
	ParserMap parsers;
	// preloading
	LoadQueue loadQueue;

	// all unique models loaded so far
	std::vector<S3DModel*> models;
};

extern C3DModelLoader* modelParser;

#endif /* IMODELPARSER_H */
