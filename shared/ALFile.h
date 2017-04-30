#pragma once
#include <Alembic/Abc/OObject.h>
#include <Alembic/Abc/OArchive.h>
#include <vector>
using namespace Alembic::Abc;
class ALTransform;
class ALMesh;
class ALFile
{
public:
    ALFile();
    ~ALFile();
    
    void openAbc(const char *filename);
	void addTimeSampling(double startTime, double endTime, double secondsPerFrame);
    OObject root();
    char findObject(const std::vector<std::string> &path, OObject &dest);
	char findObject(const std::string &fullPathName, OObject &dest);
    char findChildByName(OObject &parent, OObject &child, const std::string &name);
	char findParentOf(const std::string &fullPathName, OObject &dest);
	
	std::string terminalName(const std::string &fullPathName);
	void splitNames(const std::string &fullPathName, std::vector<std::string> &paths);

	bool addTransform(const std::string &fullPathName);
	ALTransform *lastTransform();
	
	bool addMesh(const std::string &fullPathName);
	ALMesh *lastMesh();
	
	void begin();
	ALTransform *getTransform();
	ALMesh *getMesh();
	
	void nextTransform();
	void nextMesh();
	
	unsigned numTransform() const;
	unsigned numMesh() const;
	
private:
	void flush();
	
    OArchive m_archive;
    std::vector<ALTransform *> m_transform;
	std::vector<ALMesh *> m_mesh;
	
	unsigned m_currentTransformIdx;
	unsigned m_currentMeshIdx;
	
	Alembic::AbcCoreAbstract::TimeSamplingPtr mShapeTime;
	Alembic::AbcCoreAbstract::TimeSamplingPtr mTransTime;
};
