/*
---------------------------------------------------------------------------------------------
-							       MAYA OGRE EXPORTER                                       -
---------------------------------------------------------------------------------------------
- Description: 	This is a plugin for Maya, that allows the export of animated               -
-              	meshes in the OGRE file format. All meshes will be combined                 -
-              	together to form a single OGRE mesh, each Maya mesh will be                 -
-              	translated as a submesh. Multiple materials per mesh are allowed            -
-              	each group of triangles sharing the same material will become               -
-              	a separate submesh. Skeletal animation and blendshapes are                  -
-              	supported, or, alternatively, vertex animation as a sequence                -
-              	of morph targets.                                                           -
-              	The export command can be run via script too, for instructions              -
-              	on its usage please refer to the Instructions.txt file.  					-
- Note: 		The particles exporter is an extra module submitted by the OGRE         	-
- 				community, it still has to be reviewed and fixed.  		            		-		
---------------------------------------------------------------------------------------------
- Original version by Francesco Giordana, sponsored by Anygma N.V. (http://www.nazooka.com) -
- The previous version was maintained by Filmakademie Baden-Wuerttemberg, 					-
- Institute of Animation's R&D Lab (http://research.animationsinstitut.de)  				-
-																							-
- The current version (at https://www.github.com/bitgate/maya-ogre3d-exporter) is			-
- maintained by Bitgate, Inc. for the purpose of keeping Ogre compatible with the latest	-
- technologies.																				-
---------------------------------------------------------------------------------------------
- Copyright (c) 2011 MFG Baden-Württemberg, Innovation Agency for IT and media.             -
- Research and Development at the Institute of Animation is a cooperation between           -
- MFG Baden-Württemberg, Innovation Agency for IT and media and                             -
- Filmakademie Baden-Württemberg as part of the "MFG Visual Experience Lab".                -
---------------------------------------------------------------------------------------------
- This program is free software; you can redistribute it and/or modify it under				-
- the terms of the GNU Lesser General Public License as published by the Free Software		-
- Foundation; version 2.1 of the License.													-
-																							-
- This program is distributed in the hope that it will be useful, but WITHOUT				-
- ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS				-
- FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.		-
- 																							-
- You should have received a copy of the GNU Lesser General Public License along with		-
- this program; if not, write to the Free Software Foundation, Inc., 59 Temple				-
- Place - Suite 330, Boston, MA 02111-1307, USA, or go to									-
- http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html									-
---------------------------------------------------------------------------------------------
*/

//!
//! \file "paramlist.h"
//! \brief Parameter list header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \author     Volker Helzle <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef PARAMLIST_H
#define PARAMLIST_H

#include "mayaExportLayer.h"

// Length units multipliers from Maya internal unit (cm)

#define CM2MM 10.0
#define CM2CM 1.0
#define CM2M  0.01
#define CM2IN 0.393701
#define CM2FT 0.0328084
#define CM2YD 0.0109361

namespace OgreMayaExporter
{
	class Submesh;

	typedef struct clipInfoTag
	{
		float start;							//start time of the clip
		float stop;								//end time of the clip
		float rate;								//sample rate of anim curves, -1 means auto
		MString name;							//clip name
		bool offsetAnimation;					//clip will be created as offset to the first frame of animation
	} clipInfo;

	typedef enum
	{
		NPT_CURFRAME,
		NPT_BINDPOSE
	} NeutralPoseType;

	typedef enum
	{
		TS_TEXCOORD,
		TS_TANGENT
	} TangentSemantic;

	/***** Class ParamList *****/
	class ParamList
	{
	public:
		// class members
		bool exportMesh, exportMaterial, exportAnimCurves, exportAnimCurvesXML, animSkipZeroValues, exportCameras, exportAll, exportVBA,
			exportVertNorm, exportVertCol, exportTexCoord, exportCamerasAnim, exportCamerasAnimXML,
			exportSkeleton, exportSkelAnims, exportBSAnims, optimizePoseAnimation, exportVertAnims, exportBlendShapes, 
			exportWorldCoords, useSharedGeom, lightingOff, copyTextures, exportParticles,
			buildTangents, preventZeroTangent, buildEdges, skelBB, bsBB, vertBB, 
			tangentsSplitMirrored, tangentsSplitRotated, tangentsUseParity;			

		float lum;		// Length Unit Multiplier
		float uvScale;	// UV scale factor to prevent zero tangents

		MString meshFilename, skeletonFilename, materialFilename, animFilename, camerasFilename, matPrefix,
			texOutputDir, particlesFilename;
		// xml files
		MString animFilenameXML, camerasFilenameXML;

		std::ofstream outMaterial, outAnim, outCameras, outParticles;
		// xml stream
		std::ofstream outAnimXML, outCamerasXML;

		MStringArray writtenMaterials;

		std::vector<clipInfo> skelClipList;
		std::vector<clipInfo> BSClipList;
		std::vector<clipInfo> vertClipList;

		NeutralPoseType neutralPoseType;
		TangentSemantic tangentSemantic;

		std::vector<Submesh*> loadedSubmeshes;
		std::vector<MDagPath> currentRootJoints;

		// constructor
		ParamList()	{
			lum = 1.0;
			exportMesh = false;
			exportMaterial = false;
			exportSkeleton = false;
			exportSkelAnims = false;
			exportBSAnims = false;
			optimizePoseAnimation = false;
			exportVertAnims = false;
			exportBlendShapes = false;
			exportAnimCurves = false;
			exportCameras = false;
			exportParticles = false;
			exportAll = false;
			exportWorldCoords = false;
			exportVBA = false;
			exportVertNorm = false;
			exportVertCol = false;
			exportTexCoord = false;
			exportCamerasAnim = false;
			useSharedGeom = false;
			lightingOff = false;
			copyTextures = false;
			skelBB = false;
			bsBB = false;
			vertBB = false;
			meshFilename = "";
			skeletonFilename = "";
			materialFilename = "";
			animFilename = "";
			camerasFilename = "";
			particlesFilename = "";
			matPrefix = "";
			texOutputDir = "";
			skelClipList.clear();
			BSClipList.clear();
			vertClipList.clear();
			neutralPoseType = NPT_CURFRAME;
			buildEdges = false;
			buildTangents = false;
			preventZeroTangent = false;
			uvScale = 10;
			tangentsSplitMirrored = false;
			tangentsSplitRotated = false;
			tangentsUseParity = false;
			tangentSemantic = TS_TANGENT;
			loadedSubmeshes.clear();
			currentRootJoints.clear();

			exportCamerasAnimXML = false;
			exportAnimCurvesXML = false;
			animFilenameXML = "";
			camerasFilenameXML = "";
			animSkipZeroValues = false;
		}

		ParamList& operator=(ParamList& source)	
		{
			int i;
			lum = source.lum;
			exportMesh = source.exportMesh;
			exportMaterial = source.exportMaterial;
			exportSkeleton = source.exportSkeleton;
			exportSkelAnims = source.exportSkelAnims;
			exportBSAnims = source.exportBSAnims;
			optimizePoseAnimation = source.optimizePoseAnimation;
			exportVertAnims = source.exportVertAnims;
			exportBlendShapes = source.exportBlendShapes;
			exportAnimCurves = source.exportAnimCurves;
			exportCameras = source.exportCameras;
			exportAll = source.exportAll;
			exportWorldCoords = source.exportWorldCoords;
			exportVBA = source.exportVBA;
			exportVertNorm = source.exportVertNorm;
			exportVertCol = source.exportVertCol;
			exportTexCoord = source.exportTexCoord;
			exportCamerasAnim = source.exportCamerasAnim;
			exportParticles = source.exportParticles;
			useSharedGeom = source.useSharedGeom;
			lightingOff = source.lightingOff;
			copyTextures = source.copyTextures;
			skelBB = source.skelBB;
			bsBB = source.bsBB;
			vertBB = source.vertBB;
			meshFilename = source.meshFilename;
			skeletonFilename = source.skeletonFilename;
			materialFilename = source.materialFilename;
			animFilename = source.animFilename;
			camerasFilename = source.camerasFilename;
			particlesFilename = source.particlesFilename;
			matPrefix = source.matPrefix;
			texOutputDir = source.texOutputDir;
			buildEdges = source.buildEdges;
			buildTangents = source.buildTangents;
			preventZeroTangent = source.preventZeroTangent;
			uvScale = source.uvScale;
			tangentsSplitMirrored = source.tangentsSplitMirrored;
			tangentsSplitRotated = source.tangentsSplitRotated;
			tangentsUseParity = source.tangentsUseParity;
			tangentSemantic = source.tangentSemantic;
			skelClipList.resize(source.skelClipList.size());
			for (i=0; i< skelClipList.size(); i++)
			{
				skelClipList[i].name = source.skelClipList[i].name;
				skelClipList[i].start = source.skelClipList[i].start;
				skelClipList[i].stop = source.skelClipList[i].stop;
				skelClipList[i].rate = source.skelClipList[i].rate;
			}
			BSClipList.resize(source.BSClipList.size());
			for (i=0; i< BSClipList.size(); i++)
			{
				BSClipList[i].name = source.BSClipList[i].name;
				BSClipList[i].start = source.BSClipList[i].start;
				BSClipList[i].stop = source.BSClipList[i].stop;
				BSClipList[i].rate = source.BSClipList[i].rate;
			}
			vertClipList.resize(source.vertClipList.size());
			for (i=0; i< vertClipList.size(); i++)
			{
				vertClipList[i].name = source.vertClipList[i].name;
				vertClipList[i].start = source.vertClipList[i].start;
				vertClipList[i].stop = source.vertClipList[i].stop;
				vertClipList[i].rate = source.vertClipList[i].rate;
			}
			neutralPoseType = source.neutralPoseType;
			for (i=0; i<source.loadedSubmeshes.size(); i++)
				loadedSubmeshes.push_back(source.loadedSubmeshes[i]);
			for (i=0; i<source.currentRootJoints.size(); i++)
				currentRootJoints.push_back(source.currentRootJoints[i]);
			
			exportCamerasAnimXML = source.exportCamerasAnimXML;
			exportAnimCurvesXML = source.exportAnimCurvesXML;
			animFilenameXML = source.animFilenameXML;
			camerasFilenameXML = source.camerasFilenameXML;
			animSkipZeroValues = source.animSkipZeroValues;

			return *this;
		}

		// destructor
		~ParamList() {
			if (outMaterial)
				outMaterial.close();
			if (outAnim)
				outAnim.close();
			if (outCameras)
				outCameras.close();
			if (outParticles)
				outParticles.close();
			if (outAnimXML)
				outAnimXML.close();
			if (outCamerasXML)
				outCamerasXML.close();			
		}
		// method to pars arguments and set parameters
		void parseArgs(const MArgList &args);
		// method to open files for writing
		MStatus openFiles();
		// method to close open output files
		MStatus closeFiles();
	};

};	//end namespace

#endif