/*******************************************************************************
This file is part of piccante.

piccante is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

piccante is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with piccante.  If not, see <http://www.gnu.org/licenses/>.
*******************************************************************************/

#ifndef _OUTPUT_MANAGER_H_
#define _OUTPUT_MANAGER_H_

#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS

#include "stdlib.h"
#include <mpi.h>
#include <iomanip>
#if defined(_MSC_VER)
#include <cstdint>
#else
#include <stdint.h>
#endif
#include <vector>
#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include "commons.h"
#if defined(USE_BOOST)
#include <boost/filesystem.hpp>
#endif
#if defined(USE_HDF5)
#include <hdf5.h>
#endif
#include "grid.h"
#include "structures.h"
#include "current.h"
#include "em_field.h"
#include "particle_species.h"

using namespace std;

#define OUTPUT_SIZE_TIME_DIAG 7

#define SPEC_DIAG_COMP 14
#define FIELD_DIAG_COMP 14

enum diagType{
	OUT_EM_FIELD_BINARY,

	OUT_SPEC_PHASE_SPACE_BINARY,
	OUT_DIAG,
	OUT_CURRENT_BINARY,
    OUT_EMJPROBE,
    OUT_EMJPLANE, //NON ESISTE ANCORA !
    OUT_SPEC_DENSITY_BINARY
};


struct request{
	double dtime;
	int itime;
	diagType type;
	int target;
};

struct emProbe{
    double coordinates[3];
    std::string name;
    std::string fileName;
    emProbe();
    bool compareProbes(emProbe* rhs);
    void setPointCoordinate(double X, double Y, double Z);
    void setName(std::string iname);
};

struct emPlane{
    double coordinates[3];
    bool remainingCoord[3];
    std::string name;
    emPlane();
    bool comparePlanes(emPlane* rhs);
    void setFreeDimensions(bool flagX, bool flagY, bool flagZ);
    void setPointCoordinate(double X, double Y, double Z);
    void setName(std::string iname);
};

bool requestCompTime(const request &first, const request &second);
bool requestCompUnique(const request &first, const request &second);

class OUTPUT_MANAGER
{

public:
	OUTPUT_MANAGER(GRID* _mygrid, EM_FIELD* _myfield, CURRENT* _mycurrent, std::vector<SPECIE*> _myspecies);
	~OUTPUT_MANAGER();

	void initialize(std::string _outputDir);
	void close();

	void addEMFieldBinaryFrom(double startTime, double frequency);
	void addEMFieldBinaryAt(double atTime);
	void addEMFieldBinaryFromTo(double startTime, double frequency, double endTime);

    void addEMFieldProbeFrom(emProbe* Probe, double startTime, double frequency);
    void addEMFieldProbeAt(emProbe* Probe, double atTime);
    void addEMFieldProbeFromTo(emProbe* Probe, double startTime, double frequency, double endTime);

    void addEMFieldPlaneFrom(emPlane* Plane,double startTime, double frequency);
    void addEMFieldPlaneAt(emPlane* Plane,double atTime);
    void addEMFieldPlaneFromTo(emPlane* Plane,double startTime, double frequency, double endTime);

    void addSpecDensityBinaryFrom(std::string name, double startTime, double frequency);
	void addSpecDensityBinaryAt(std::string name, double atTime);
	void addSpecDensityBinaryFromTo(std::string name, double startTime, double frequency, double endTime);

	void addCurrentBinaryFrom(double startTime, double frequency);
	void addCurrentBinaryAt(double atTime);
	void addCurrentBinaryFromTo(double startTime, double frequency, double endTime);

	void addSpecPhaseSpaceBinaryFrom(std::string name, double startTime, double frequency);
	void addSpecPhaseSpaceBinaryAt(std::string name, double atTime);
	void addSpecPhaseSpaceBinaryFromTo(std::string name, double startTime, double frequency, double endTime);

	void addDiagFrom(double startTime, double frequency);
	void addDiagAt(double atTime);
	void addDiagFromTo(double startTime, double frequency, double endTime);


	void autoVisualDiag();

	void callDiags(int istep);



	//    void addEMFieldStat(int start, int frequency, EM_FIELD* field_pointer);
	//    void addEMFieldStatAtTime(int tstepout, EM_FIELD* field_pointer);





	//    void addSpecStat(int start, int frequency, SPECIE* spec );
	//    void addSpecStatAtTime(int tstepout, SPECIE* spec);

	//    void addStat(int start, int frequency, EM_FIELD* aux, std::vector<SPECIE*>* spec_vec, bool simple);
	//    void addStatAtTime(int tstepout, EM_FIELD* aux, std::vector<SPECIE*>* spec_vec, bool simple);



	//    void addEMJProbe(int start, int frequency, double x, double y, double z, EM_FIELD* field_pointer, CURRENT* current_pointer);
	//    void addEMJProbeAtTime(int tstepout, double x, double y, double z, EM_FIELD* field_pointer, CURRENT* current_pointer);

private:
	GRID* mygrid;
	EM_FIELD* myfield;
	CURRENT* mycurrent;
	std::vector<SPECIE*> myspecies;
    std::vector<emProbe*> myEMProbes;
    std::vector<emPlane*> myEMPlanes;

	bool isThereGrid;
	bool isThereCurrent;
	bool isThereField;
	bool isThereSpecList;

	std::string outputDir;
	bool amIInit;

	bool isThereDiag;
    bool isThereEMProbe;

	bool checkGrid();
	bool checkEMField();
	bool checkCurrent();
	bool checkSpecies();
    bool isInMyDomain(double *rr);
    void nearestInt(double *rr, int *ri, int *globalri);
    int findSpecName(std::string name);
    int returnTargetIfProbeIsInList(emProbe *newProbe);
    int returnTargetIfPlaneIsInList(emPlane *newPlane);
    std::string diagFileName;
	std::string extremaFieldFileName;
	std::vector<std::string> extremaSpecFileNames;

	std::list<request> requestList;
	std::vector<int> timeList;
	std::map< int, std::vector<request> > allOutputs;

	static const int diagWideWidth = 16;
	static const int diagNarrowWidth = 6;
	static const int diagWidth = 10;

	int getIntTime(double dtime);

	void addRequestToList(std::list<request>& timeList, diagType type, int target, double startTime, double frequency, double endTime);

	void prepareOutputMap();

	void createDiagFile();
	void createExtremaFiles();
    void createEMProbeFiles();

	void processOutputEntry(request req);

	std::string composeOutputName(std::string dir, std::string out, std::string opt, double time, std::string ext);
	void writeEMFieldMap(std::ofstream &output, request req);
	void writeEMFieldBinary(std::string fileName, request req);
    void writeEMFieldBinaryHDF5(std::string fileName, request req);
    void callEMFieldBinary(request req);

    void callEMFieldProbe(request req);
    void writeEMFieldPlane(std::string fileName, emPlane *plane);
    void callEMFieldPlane(request req);

    void writeSpecDensityMap(std::ofstream &output, request req);
	void writeSpecDensityBinary(std::string fileName, request req);
	void callSpecDensityBinary(request req);

	void writeCurrentMap(std::ofstream &output, request req);
	void writeCurrentBinary(std::string fileName, request req);
	void callCurrentBinary(request req);

	void writeSpecPhaseSpaceBinary(std::string fileName, request req);
	void callSpecPhaseSpaceBinary(request req);


	void callDiag(request req);
    void interpEB(double pos[3], double E[3], double B[3]);
};

#endif // DIAG_MANAGER_PLUS_H

