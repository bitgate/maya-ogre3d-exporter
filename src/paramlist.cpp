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
//! \file "paramlist.cpp"
//! \brief Parameter list for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \author     Volker Helzle <marcel.reinhardt@filmakademie.de>
//! \author		Bart Pelle <bart@bitgate.com>
//! \version    1.1
//! \date       7.12.2017 (last updated)
//!

#include "paramlist.h"
#include <maya/MGlobal.h>

/***** Class ParamList *****/
// method to parse arguments from command line
namespace OgreMayaExporter
{
	void ParamList::parseArgs(const MArgList &args)
	{
		MStatus stat;
		// Parse arguments from command line
		for (uint i=0; i < args.length(); i++ )
		{
			if ((MString("-all") == args.asString(i,&stat)) && (MS::kSuccess == stat))
				exportAll = true;
			else if ((MString("-world") == args.asString(i,&stat)) && (MS::kSuccess == stat))
				exportWorldCoords = true;
			else if ((MString("-lu") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				MString lengthUnit = args.asString(++i,&stat);
				if (MString("pref") == lengthUnit)
				{
					MGlobal::executeCommand("currentUnit -q -l",lengthUnit,false);
				}
				if (MString("mm") == lengthUnit)
					lum = CM2MM;
				else if (MString("cm") == lengthUnit)
					lum = CM2CM;
				else if (MString("m") == lengthUnit)
					lum = CM2M;
				else if (MString("in") == lengthUnit)
					lum = CM2IN;
				else if (MString("ft") == lengthUnit)
					lum = CM2FT;
				else if (MString("yd") == lengthUnit)
					lum = CM2YD;
			}
			else if ((MString("-scale") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				float s = args.asDouble(++i,&stat);
				lum *= s;
			}
			else if ((MString("-mesh") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportMesh = true;
				meshFilename = args.asString(++i,&stat);
			}
			else if ((MString("-mat") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportMaterial = true;
				materialFilename = args.asString(++i,&stat);
			}
			else if ((MString("-matPrefix") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				matPrefix = args.asString(++i,&stat);
			}
			else if ((MString("-copyTex") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				copyTextures = true;
				texOutputDir = args.asString(++i,&stat);
			}
			else if ((MString("-lightOff") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				lightingOff = true;
			}
			else if ((MString("-skel") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportSkeleton = true;
				skeletonFilename = args.asString(++i,&stat);
			}
			else if ((MString("-skeletonAnims") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportSkelAnims = true;
			}
			else if ((MString("-vertexAnims") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportVertAnims = true;
			}
			else if ((MString("-blendShapes") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportBlendShapes = true;
			}
			else if ((MString("-BSAnims") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportBSAnims = true;
			}
			else if ((MString("-optimizePoseAnimation") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				optimizePoseAnimation = true;
			}
			else if ((MString("-skelBB") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				skelBB = true;
			}
			else if ((MString("-bsBB") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				bsBB = true;
			}
			else if ((MString("-vertBB") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				vertBB = true;
			}
			else if ((MString("-animCur") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportAnimCurves = true;
				animFilename = args.asString(++i,&stat);
			}
			else if ((MString("-animCurXML") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportAnimCurvesXML = true;
				animFilenameXML = args.asString(++i,&stat);
			}
			else if ((MString("-animSkipZeroValues") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				animSkipZeroValues = true;
			}
			else if ((MString("-cam") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportCameras = true;
				camerasFilename = args.asString(++i,&stat);
			}
			else if ((MString("-v") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportVBA = true;
			}
			else if ((MString("-n") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportVertNorm = true;
			}
			else if ((MString("-c") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportVertCol = true;
			}
			else if ((MString("-t") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportTexCoord = true;
			}
			else if ((MString("-edges") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				buildEdges = true;
			}
			else if ((MString("-tangents") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				buildTangents = true;
				MString tanSem = args.asString(++i,&stat);
				if (tanSem == "TEXCOORD")
					tangentSemantic = TS_TEXCOORD;
				else if (tanSem == "TANGENT")
					tangentSemantic = TS_TANGENT;
			}
			else if ((MString("-preventZeroTangent") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				preventZeroTangent = true;
				float s = args.asDouble(++i,&stat);
				if (!s==0) uvScale = s;
			}
			else if ((MString("-tangentsplitmirrored") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				tangentsSplitMirrored = true;
			}
			else if ((MString("-tangentsplitrotated") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				tangentsSplitRotated = true;
			}
			else if ((MString("-tangentuseparity") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				tangentsUseParity = true;
			}
			else if ((MString("-camAnim") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportCamerasAnim = true;
			}
			else if ((MString("-camAnimXML") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportCamerasAnimXML = true;
				camerasFilenameXML = args.asString(++i,&stat);
			}
			else if ((MString("-particles") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				exportParticles = true;
				particlesFilename = args.asString(++i,&stat);
			}
			else if ((MString("-shared") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				useSharedGeom = true;
			}
			else if ((MString("-np") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				MString npType = args.asString(++i,&stat);
				if (npType == "curFrame")
					neutralPoseType = NPT_CURFRAME;
				else if (npType == "bindPose")
					neutralPoseType = NPT_BINDPOSE;
			}
			else if ((MString("-skeletonClip") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				//get clip name
				MString clipName = args.asString(++i,&stat);
				//check if name is unique, otherwise skip the clip
				bool uniqueName = true;
				for (int k=0; k<skelClipList.size() && uniqueName; k++)
				{
					if (clipName == skelClipList[k].name)
						uniqueName = false;
				}
				//if the name is uniue, load the clip info
				if (uniqueName)
				{
					//get clip range
					MString clipRangeType = args.asString(++i,&stat);
					float startTime, stopTime;
					if (clipRangeType == "startEnd")
					{
						startTime = args.asDouble(++i,&stat);
						stopTime = args.asDouble(++i,&stat);
						MString rangeUnits = args.asString(++i,&stat);
						if (rangeUnits == "frames")
						{
							//range specified in frames => convert to seconds
							MTime t1(startTime, MTime::uiUnit());
							MTime t2(stopTime, MTime::uiUnit());
							startTime = t1.as(MTime::kSeconds);
							stopTime = t2.as(MTime::kSeconds);
						}
					}
					else
					{
						//range specified by time slider
						MTime t1 = MAnimControl::minTime();
						MTime t2 = MAnimControl::maxTime();
						startTime = t1.as(MTime::kSeconds);
						stopTime = t2.as(MTime::kSeconds);
					}
					// get sample rate
					float rate;
					MString sampleRateType = args.asString(++i,&stat);
					if (sampleRateType == "sampleByFrames")
					{
						// rate specified in frames
						int intRate = args.asInt(++i,&stat);
						MTime t = MTime(intRate, MTime::uiUnit());
						rate = t.as(MTime::kSeconds);
					}
					else
					{
						// rate specified in seconds
						rate = args.asDouble(++i,&stat);
					}
					//add clip info
					clipInfo clip;
					clip.name = clipName;
					clip.start = startTime;
					clip.stop = stopTime;
					clip.rate = rate;
					skelClipList.push_back(clip);
					std::cout << "skeleton clip " << clipName.asChar() << "\n";
					std::cout << "start: " << startTime << ", stop: " << stopTime << "\n";
					std::cout << "rate: " << rate << "\n";
					std::cout << "-----------------\n";
					std::cout.flush();
				}
				//warn of duplicate clip name
				else
				{
					std::cout << "Warning! A skeleton clip with name \"" << clipName.asChar() << "\" already exists\n";
					std::cout.flush();
				}
			}
			else if ((MString("-BSClip") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				//get clip name
				MString clipName = args.asString(++i,&stat);
				//check if name is unique, otherwise skip the clip
				bool uniqueName = true;
				for (int k=0; k<BSClipList.size() && uniqueName; k++)
				{
					if (clipName == BSClipList[k].name)
						uniqueName = false;
				}
				//if the name is uniue, load the clip info
				if (uniqueName)
				{
					//get clip range
					MString clipRangeType = args.asString(++i,&stat);
					float startTime, stopTime;
					if (clipRangeType == "startEnd")
					{
						startTime = args.asDouble(++i,&stat);
						stopTime = args.asDouble(++i,&stat);
						MString rangeUnits = args.asString(++i,&stat);
						if (rangeUnits == "frames")
						{
							//range specified in frames => convert to seconds
							MTime t1(startTime, MTime::uiUnit());
							MTime t2(stopTime, MTime::uiUnit());
							startTime = t1.as(MTime::kSeconds);
							stopTime = t2.as(MTime::kSeconds);
						}
					}
					else
					{
						//range specified by time slider
						MTime t1 = MAnimControl::minTime();
						MTime t2 = MAnimControl::maxTime();
						startTime = t1.as(MTime::kSeconds);
						stopTime = t2.as(MTime::kSeconds);
					}
					// get sample rate
					float rate;
					MString sampleRateType = args.asString(++i,&stat);
					if (sampleRateType == "sampleByFrames")
					{
						// rate specified in frames
						int intRate = args.asInt(++i,&stat);
						MTime t = MTime(intRate, MTime::uiUnit());
						rate = t.as(MTime::kSeconds);
					}
					else
					{
						// rate specified in seconds
						rate = args.asDouble(++i,&stat);
					}
					//add clip info
					clipInfo clip;
					clip.name = clipName;
					clip.start = startTime;
					clip.stop = stopTime;
					clip.rate = rate;
					BSClipList.push_back(clip);
					std::cout << "blend shape clip " << clipName.asChar() << "\n";
					std::cout << "start: " << startTime << ", stop: " << stopTime << "\n";
					std::cout << "rate: " << rate << "\n";
					std::cout << "-----------------\n";
					std::cout.flush();
				}
				//warn of duplicate clip name
				else
				{
					std::cout << "Warning! A blend shape clip with name \"" << clipName.asChar() << "\" already exists\n";
					std::cout.flush();
				}
			}
			else if ((MString("-vertexClip") == args.asString(i,&stat)) && (MS::kSuccess == stat))
			{
				//get clip name
				MString clipName = args.asString(++i,&stat);
				//check if name is unique, otherwise skip the clip
				bool uniqueName = true;
				for (int k=0; k<vertClipList.size() && uniqueName; k++)
				{
					if (clipName == vertClipList[k].name)
						uniqueName = false;
				}
				//if the name is uniue, load the clip info
				if (uniqueName)
				{
				//get clip range
					MString clipRangeType = args.asString(++i,&stat);
					float startTime, stopTime;
					if (clipRangeType == "startEnd")
					{
						startTime = args.asDouble(++i,&stat);
						stopTime = args.asDouble(++i,&stat);
						MString rangeUnits = args.asString(++i,&stat);
						if (rangeUnits == "frames")
						{
							//range specified in frames => convert to seconds
							MTime t1(startTime, MTime::uiUnit());
							MTime t2(stopTime, MTime::uiUnit());
							startTime = t1.as(MTime::kSeconds);
							stopTime = t2.as(MTime::kSeconds);
						}
					}
					else
					{
						//range specified by time slider
						MTime t1 = MAnimControl::minTime();
						MTime t2 = MAnimControl::maxTime();
						startTime = t1.as(MTime::kSeconds);
						stopTime = t2.as(MTime::kSeconds);
					}
					// get sample rate
					float rate;
					MString sampleRateType = args.asString(++i,&stat);
					if (sampleRateType == "sampleByFrames")
					{
						// rate specified in frames
						int intRate = args.asInt(++i,&stat);
						MTime t = MTime(intRate, MTime::uiUnit());
						rate = t.as(MTime::kSeconds);
					}
					else
					{
						// rate specified in seconds
						rate = args.asDouble(++i,&stat);
					}
					//add clip info
					clipInfo clip;
					clip.name = clipName;
					clip.start = startTime;
					clip.stop = stopTime;
					clip.rate = rate;
					vertClipList.push_back(clip);
					std::cout << "vertex clip " << clipName.asChar() << "\n";
					std::cout << "start: " << startTime << ", stop: " << stopTime << "\n";
					std::cout << "rate: " << rate << "\n";
					std::cout << "-----------------\n";
					std::cout.flush();
				}
				//warn of duplicate clip name
				else
				{
					std::cout << "Warning! A vertex animation clip with name \"" << clipName.asChar() << "\" already exists\n";
					std::cout.flush();
				}
			}
			else if (MString("-version") == args.asString(i, &stat) && (MS::kSuccess == stat))
			{
				MString versionName = args.asString(++i, &stat);

				if (versionName == "latest")
				{
					targetMeshVersion = Ogre::MESH_VERSION_LATEST;
				}
				else if (versionName == "1.10")
				{
					targetMeshVersion = Ogre::MESH_VERSION_1_10;
				}
				else if (versionName == "1.8")
				{
					targetMeshVersion = Ogre::MESH_VERSION_1_8;
				}
				else if (versionName == "1.7")
				{
					targetMeshVersion = Ogre::MESH_VERSION_1_7;
				}
				else if (versionName == "1.4")
				{
					targetMeshVersion = Ogre::MESH_VERSION_1_4;
				}
				else if (versionName == "1.0")
				{
					targetMeshVersion = Ogre::MESH_VERSION_1_0;
				}
				else
				{
					std::cout << "Warning: unknown mesh export version \"" << versionName.asChar() << "\", defaulting to latest. Available versions:\n";
					std::cout << "    latest: Mesh version 1.10 (always resolves to latest available version)\n";
					std::cout << "    1.10: Mesh version 1.10 (internally 1.100)\n";
					std::cout << "    1.8: Mesh version 1.8 (internally 1.8)\n";
					std::cout << "    1.7: Mesh version 1.7 (internally 1.41)\n";
					std::cout << "    1.4: Mesh version 1.4 (internally 1.40)\n";
					std::cout << "    1.0: Mesh version 1.0 (internally 1.30)\n";
					std::cout.flush();
				}
			}
		}
	}


	// method to open output files for writing
	MStatus ParamList::openFiles()
	{
		MString msg;
		if (exportMaterial)
		{
			outMaterial.open(materialFilename.asChar());
			if (!outMaterial)
			{
				std::cout << "Error opening file: " << materialFilename.asChar() << "\n";
				return MS::kFailure;
			}
		}
		if (exportAnimCurves)
		{
			outAnim.open(animFilename.asChar());
			if (!outAnim)
			{
				std::cout << "Error opening file: " << animFilename.asChar() << "\n";
				return MS::kFailure;
			}

		}
		if (exportCameras)
		{
			outCameras.open(camerasFilename.asChar());
			if (!outCameras)
			{
				std::cout << "Error opening file: " << camerasFilename.asChar() << "\n";
				return MS::kFailure;
			}
		}
		if (exportAnimCurvesXML)
		{
			outAnimXML.open(animFilenameXML.asChar());
			if (!outAnimXML)
			{
				std::cout << "Error opening file: " << animFilenameXML.asChar() << "\n";
				return MS::kFailure;
			}

		}
		if (exportCamerasAnimXML)
		{
			outCamerasXML.open(camerasFilenameXML.asChar());
			if (!outCamerasXML)
			{
				std::cout << "Error opening file: " << camerasFilenameXML.asChar() << "\n";
				return MS::kFailure;
			}
		}
		if (exportParticles)
		{
			outParticles.open(particlesFilename.asChar());
			if (!outParticles)
			{
				std::cout << "Error opening file: " << particlesFilename.asChar() << "\n";
				return MS::kFailure;
			}
		}
		return MS::kSuccess;
	}

	// method to close open output files
	MStatus ParamList::closeFiles()
	{
		if (exportMaterial)
			outMaterial.close();

		if (exportAnimCurves)
			outAnim.close();

		if (exportCameras)
			outCameras.close();

		if (exportAnimCurvesXML)
			outAnimXML.close();

		if (exportCamerasAnimXML)
			outCamerasXML.close();

		return MS::kSuccess;
	}

}	//end namespace
