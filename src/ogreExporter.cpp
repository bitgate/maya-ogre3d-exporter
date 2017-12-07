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
//! \file "ogreExporter.cpp"
//! \brief Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \author     Volker Helzle <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!
#include <OgreExporter.h>
#include <iostream>
namespace OgreMayaExporter
{
	OgreExporter::~OgreExporter()
	{
		exit();
	}
	// Restore the scene to a state previous to the export, clean up memory and exit
	void OgreExporter::exit()
	{
		// Restore active selection list
		MGlobal::setActiveSelectionList(m_selList);
		// Restore current time
		MAnimControl::setCurrentTime(m_curTime);
		// Free memory
		delete m_pMesh;
		m_pMesh = 0;
		delete m_pMaterialSet;
		m_pMaterialSet = 0;
		// Close output files
		m_params.closeFiles();
		std::cout.flush();
	}
	// Execute the command
	MStatus OgreExporter::doIt(const MArgList& args)
	{
		// clean up
		delete m_pMesh;
		delete m_pMaterialSet;
		_vA_map.clear();
		_firstStart = 1e12;
		_lastStop = -1.;
		// Parse the arguments.
		m_params.parseArgs(args);
		// Create output files
		m_params.openFiles();
		// Create a new empty mesh
		m_pMesh = new Mesh();
		// Create a new empty material set
		m_pMaterialSet = new MaterialSet();
		// Save current time for later restore
		m_curTime = MAnimControl::currentTime();
		// Save active selection list for later restore
		MGlobal::getActiveSelectionList(m_selList);
		/**************************** LOAD DATA **********************************/		
		if (m_params.exportAll)
		{	// We are exporting the whole scene					
			std::cout << "Export the whole scene\n";
			std::cout.flush();
			MItDag dagIter;
			MFnDagNode worldDag (dagIter.root());
			MDagPath worldPath;
			worldDag.getPath(worldPath);
			stat = translateNode(worldPath);
		}
		else
		{	// We are translating a selection
			std::cout << "Export selected objects\n";
			std::cout.flush();
			// Get the selection list
			MSelectionList activeList;
			stat = MGlobal::getActiveSelectionList(activeList);
			if (MS::kSuccess != stat)
			{
				std::cout << "Error retrieving selection list\n";
				std::cout.flush();
				exit();
				return MS::kFailure;
			}
			MItSelectionList iter(activeList);
			for ( ; !iter.isDone(); iter.next())
			{								
				MDagPath dagPath;
				stat = iter.getDagPath(dagPath);
				stat = translateNode(dagPath); 
			}							
		}
		// Load vertex animations
		if (m_params.exportVertAnims)
			m_pMesh->loadAnims(m_params);
		// Load blend shapes
		if (m_params.exportBlendShapes)
			m_pMesh->loadBlendShapes(m_params);
		// Restore skeleton to correct pose
		if (m_pMesh->getSkeleton())
			m_pMesh->getSkeleton()->restorePose();
		// Load skeleton animation (do it now, so we have loaded all needed joints)
		if (m_pMesh->getSkeleton() && m_params.exportSkelAnims)
		{
			// Load skeleton animations
			m_pMesh->getSkeleton()->loadAnims(m_params);
		}
		/**************************** WRITE DATA **********************************/
		stat = writeOgreData();
		if(stat == MS::kFailure)
		{
			std::cout << "Error in Ogre Export\n";
			std::cout.flush();
		}
		stat = writeClipLengths(m_params);
		if(stat == MS::kFailure)
		{
			std::cout << "Error in Writing of clip lengths\n";
			std::cout.flush();
		}
		std::cout << "Export completed succesfully\n";
		std::cout.flush();
		exit();
		return MS::kSuccess;
	}
	/**************************** TRANSLATE A NODE **********************************/
	// Method for iterating over nodes in a dependency graph from top to bottom
	MStatus OgreExporter::translateNode(MDagPath& dagPath)
	{
		if (m_params.exportAnimCurves || m_params.exportAnimCurvesXML)
		{
			MObject dagPathNode = dagPath.node();
			MItDependencyGraph animIter( dagPathNode,
				MFn::kAnimCurve,
				MItDependencyGraph::kUpstream,
				MItDependencyGraph::kDepthFirst,
				MItDependencyGraph::kNodeLevel,
				&stat );
			if (stat)
			{
				for (; !animIter.isDone(); animIter.next())
				{
					MObject anim = animIter.thisNode(&stat);
					MFnAnimCurve animFn(anim,&stat);
					std::cout << "Found animation curve:\t\t" << animFn.name().asChar() << "\n";
					std::cout << "Translating animation curve:\t" << animFn.name().asChar() << "...\n";
					std::cout.flush();
					stat = writeAnim(animFn);
					if (MS::kSuccess == stat)
					{
						std::cout << "OK\n";
						std::cout.flush();
					}
					else
					{
						std::cout << "Error, Aborting operation\n";
						std::cout.flush();
						return MS::kFailure;
					}
				}
			}
		}
		if (dagPath.hasFn(MFn::kMesh)&&(m_params.exportMesh||m_params.exportMaterial||m_params.exportSkeleton)
			&& (dagPath.childCount() == 0))
		{	// we have found a mesh shape node, it can't have any children, and it contains
			// all the mesh geometry data
			MDagPath meshDag = dagPath;
			MFnMesh meshFn(meshDag);
			if (!meshFn.isIntermediateObject())
			{
				std::cout << "Found mesh node: " << meshDag.fullPathName().asChar() << "\n";
				std::cout << "Loading mesh node " << meshDag.fullPathName().asChar() << "...\n";
				std::cout.flush();
				stat = m_pMesh->load(meshDag,m_params);
				if (MS::kSuccess == stat)
				{
					std::cout << "OK\n";
					std::cout.flush();
				}
				else
				{
					std::cout << "Error, mesh skipped\n";
					std::cout.flush();
				}
			}
		}
		else if (dagPath.hasFn(MFn::kCamera)&&(m_params.exportCameras) && (!dagPath.hasFn(MFn::kTransform)))
		{	// we have found a camera shape node, it can't have any children, and it contains
			// all information about the camera
			MFnCamera cameraFn(dagPath);
			if (!cameraFn.isIntermediateObject())
			{
				std::cout <<  "Found camera node: "<< dagPath.fullPathName().asChar() << "\n";
				std::cout <<  "Translating camera node: "<< dagPath.fullPathName().asChar() << "...\n";
				std::cout.flush();
				stat = writeCamera(cameraFn);
				if (MS::kSuccess == stat)
				{
					std::cout << "OK\n";
					std::cout.flush();
				}
				else
				{
					std::cout << "Error, Aborting operation\n";
					std::cout.flush();
					return MS::kFailure;
				}
			}
		}
		else if ( ( dagPath.apiType() == MFn::kParticle ) && m_params.exportParticles )
		{	// we have found a set of particles
			MFnDagNode fnNode(dagPath);
			if (!fnNode.isIntermediateObject())
			{
				std::cout <<  "Found particles node: "<< dagPath.fullPathName().asChar() << "\n";
				std::cout <<  "Translating particles node: "<< dagPath.fullPathName().asChar() << "...\n";
				std::cout.flush();
				Particles particles;
				particles.load(dagPath,m_params);
				stat = particles.writeToXML(m_params);
				if (MS::kSuccess == stat)
				{
					std::cout << "OK\n";
					std::cout.flush();
				}
				else
				{
					std::cout << "Error, Aborting operation\n";
					std::cout.flush();
					return MS::kFailure;
				}
			}
		}
		// look for meshes and cameras within the node's children
		for (uint i=0; i<dagPath.childCount(); i++)
		{
			MObject child = dagPath.child(i);
			 MDagPath childPath = dagPath;
			stat = childPath.push(child);
			if (MS::kSuccess != stat)
			{
				std::cout << "Error retrieving path to child " << i << " of: " << dagPath.fullPathName().asChar();
				std::cout.flush();
				return MS::kFailure;
			}
			stat = translateNode(childPath);
			if (MS::kSuccess != stat)
				return MS::kFailure;
		}
		return MS::kSuccess;
	}
	/********************************************************************************************************
	*                       Method to translate a single animation curve                                   *
	********************************************************************************************************/
	MStatus OgreExporter::writeAnim(MFnAnimCurve& anim)
	{
		MString name = anim.name();
		MString outString = "";
		unsigned int strLength = name.length();
		int c_pos = name.index('cam');		
		bool is_verticalAperture = false; 
		bool is_focalLength = false; 
		if(c_pos>-1) {
			//cout << "Found cam in name" << endl;
			MStringArray splitName(10,"");
			name.split('_',splitName);
			unsigned int splitLength = splitName.length();
			unsigned int currentLength = splitLength;
			for(unsigned int i=0;i<currentLength;) {
				if((splitName)[i]=="") {
			//		cout << "Removed: " << (splitName)[i] << " from array" << endl;
					splitName.remove(i);
					currentLength--;
				}
				else
					i++;
			}
			splitLength = splitName.length();
			//for(unsigned int i=0;i<splitLength;i++) {
			//	cout << (splitName)[i].asChar() << endl;
			//}
			MString chanName = splitName[splitLength-1];
			unsigned int chanNameLength = chanName.length();
			//cout << "Channel name/length: " << chanName << ", " << chanNameLength << endl;	
			if(chanName == "verticalFilmAperture") { // For calculation of FOVy, we need the focal length first which is exported after vA -> need to store the vA
				is_verticalAperture = true;
				if(_vA_map.size() > 0)
					_vA_map.clear();
			}
			if(chanName == "focalLength") { // later write out FOVy when knowing the focal length
				is_focalLength = true;				
			}
			//cout << (chanName.asChar())[0] << endl;
            char *chanNameChar = new char[chanNameLength + 1];
			for(unsigned int i=0;i<(chanNameLength-1);i++) {
				chanNameChar[i] = (chanName.asChar())[i];
			}
            chanNameChar[chanNameLength-1] = '\0';
            //cout << chanNameChar << endl;
			MString stripString(chanNameChar);
            delete[] chanNameChar;
			//cout << stripString << endl;
			if(stripString=="translate" || stripString=="rotate" || stripString=="scale") {
				outString = stripString + "_" + chanName;
			}
			else {
				outString = chanName;
			}
		}
		else {
			outString = anim.name(); 
		}
		// Pretest for zero only values
		bool zeroValue = false;	
		if (m_params.animSkipZeroValues){			
			for (uint i=0; i<anim.numKeys(); i++) {
				if ( anim.value(i) > 0) zeroValue = true;
			}
		}
		if (!zeroValue && m_params.animSkipZeroValues) {
			std::cout << "Skipping Animation Curve:\t" << outString.asChar() << " (containing only zero values)\n";			
			std::cout.flush();
			}	
		// export anims
		else {		
		m_params.outAnim << "anim " << outString.asChar() << "\n";
		m_params.outAnim <<"{\n";
		m_params.outAnim << "\t//Time   /    Value\n";
		float animStart = anim.time(0).as(MTime::kSeconds);
		float animStop = anim.time(anim.numKeys()-1).as(MTime::kSeconds);
		
		m_params.outAnimXML << "\t<animation name=\"" << outString.asChar() << "\" type=\"float\" start=\"" <<  animStart << "\" end=\"" << animStop << "\">\n" ;
		for (uint i=0; i<anim.numKeys(); i++) {
			if(anim.time(i).as(MTime::kSeconds)<_firstStart)
				_firstStart = anim.time(i).as(MTime::kSeconds);
			if(anim.time(i).as(MTime::kSeconds)>_lastStop)
				_lastStop = anim.time(i).as(MTime::kSeconds);
			m_params.outAnim << "\t" << anim.time(i).as(MTime::kSeconds) << "\t" << anim.value(i) << "\n";
			m_params.outAnimXML << "\t\t<keyframe time=\"" << anim.time(i).as(MTime::kSeconds) << "\" value=\"" << anim.value(i) << "\"/>\n";
			if(is_verticalAperture == true) {
				_vA_map.insert(std::make_pair(anim.time(i).as(MTime::kSeconds), anim.value(i)));
			}
		}
		m_params.outAnim << "}\n\n";
		m_params.outAnimXML << "\t</animation>\n";
		// in case we are treating the horizontal aperture, also write the correct FOVy for Ogre
		if(is_focalLength == true) {
			cout << "Adding FOVy..." << endl;
			m_params.outAnim << "anim FOVy" << "\n";
			m_params.outAnim <<"{\n";
			m_params.outAnim << "\t//Time   /    Value\n";
			std::map<float,float>::iterator av_it;
			if(anim.numKeys()>_vA_map.size()) {			
						// TODO
				}				
			else {
				for(av_it=_vA_map.begin();av_it!=_vA_map.end();av_it++) {			
					float last_key = 0.0;
					float take_this_key = -1.0;
					for(uint i=0; i<anim.numKeys(); i++) {
						if(abs(anim.time(i).as(MTime::kSeconds) - av_it->first)<0.0001) {
							take_this_key = anim.value(i);
							break;
						}
						else {
							last_key = anim.value(i);
						}
					}
					float vA_val = av_it->second;
					float focalLength = -1. ; 
					if(take_this_key > -1.1) 
						focalLength = take_this_key;
					else
						focalLength = last_key;
					float FOVy = 2*atan(vA_val*25.4/(2*focalLength)) * 180 / 3.141592654; 
					m_params.outAnim << "\t" << av_it->first << "\t" << FOVy << "\n";
				}
			}
			m_params.outAnim << "}\n\n";
		}
		}
		return MS::kSuccess;
	
	}
	/********************************************************************************************************
	*                           Method to translate a single camera                                        *
	********************************************************************************************************/
	MStatus OgreExporter::writeCamera(MFnCamera& camera)
	{
		std::cout << "Exporting a camera" << std::endl;
		MPlug plug;
		MPlugArray srcplugarray;
		double dist;
		MAngle angle;
		MFnTransform* cameraTransform = NULL;
		MFnAnimCurve* animCurve = NULL;
		MFnAnimCurve* animCurve_x = NULL;
		MFnAnimCurve* animCurve_y = NULL;
		MFnAnimCurve* animCurve_z = NULL;
		_firstStart_cam = 1e12;
		_lastStop_cam = -1.;
		float firstStart_cam_local = 1e12;
		float lastStop_cam_local = -1.;
		std::stringbuf sbuf;
		std::iostream outCamerasXML_tmp(&sbuf);
		std::map<MTime, double* > keyMap;
		std::map<MTime, double* >::iterator keyMap_it;
		std::map<MTime, double* >::iterator local_it;
		// get camera transform
		for (int i=0; i<camera.parentCount(); i++)
		{
			if (camera.parent(i).hasFn(MFn::kTransform))
			{
				cameraTransform = new MFnTransform(camera.parent(i));
				continue;
			}
		}
		// List plugs
		MPlugArray plugs;
		MStatus stat = cameraTransform->getConnections(plugs);
		cout << "Connections:" << endl;
		for(unsigned int i=0; i< plugs.length(); i++)
			cout << (plugs)[i].name() << endl;
		// start camera description
		m_params.outCameras << "camera " << cameraTransform->partialPathName().asChar() << "\n";
		m_params.outCameras << "{\n";
		// write translation data
		m_params.outCameras << "\ttranslation\n";
		m_params.outCameras << "\t{\n";
		//translateX
		m_params.outCameras << "\t\tx ";
		plug = cameraTransform->findPlug("translateX");
		if (plug.isConnected() && m_params.exportCamerasAnimXML)
		{
			plug.connectedTo(srcplugarray,true,false,&stat);
			for (int i=0; i < srcplugarray.length(); i++)
			{
				if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
				{
					if (animCurve_x)
						delete animCurve_x;
					animCurve_x = new MFnAnimCurve(srcplugarray[i].node());
					continue;
				}
				else if (i == srcplugarray.length()-1)
				{
					std::cout << "Invalid link to translateX attribute\n";
					return MS::kFailure;
				}
			}
			float animStart = animCurve_x->time(0).as(MTime::kSeconds);
			float animStop = animCurve_x->time(animCurve_x->numKeys()-1).as(MTime::kSeconds);		
			if(animStart<_firstStart_cam)
				_firstStart_cam = animStart;
			if(animStop>_lastStop_cam)
				_lastStop_cam = animStop;
			if(animStart<firstStart_cam_local)
				firstStart_cam_local = animStart;
			if(animStop>lastStop_cam_local)
				lastStop_cam_local = animStop;
			m_params.outCameras << "anim " << animCurve_x->name().asChar() << "\n";
			m_params.outCameras <<"\t\t\t{\n";
			m_params.outCameras << "\t\t\t//Time   /    Value\n";
		
			for (uint i=0; i<animCurve_x->numKeys(); i++) {
				m_params.outCameras << "\t\t\t" << animCurve_x->time(i).as(MTime::kSeconds) << "\t" << animCurve_x->value(i) << "\n";
				// x is reference for the anim map
				double *keyVals = new double[3];
				keyVals[0] = animCurve_x->value(i);
				keyVals[1] = 0.0;
				keyVals[2] = 0.0;
				keyMap.insert(std::make_pair( animCurve_x->time(i), keyVals) );
			}
			m_params.outCameras << "\t\t}\n\n";
		}
		else
		{
			plug.getValue(dist);
			m_params.outCameras << "= " << dist << "\n";
		}
		//translateY
		m_params.outCameras << "\t\ty ";
		plug = cameraTransform->findPlug("translateY");
		if (plug.isConnected() && m_params.exportCamerasAnimXML)
		{
			plug.connectedTo(srcplugarray,true,false,&stat);
			for (int i=0; i< srcplugarray.length(); i++)
			{
				if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
				{
					if (animCurve_y)
						delete animCurve_y;
					animCurve_y = new MFnAnimCurve(srcplugarray[i].node());
					continue;
				}
				else if (i == srcplugarray.length()-1)
				{
					std::cout << "Invalid link to translateY attribute\n";
					return MS::kFailure;
				}
			}
			float animStart = animCurve_y->time(0).as(MTime::kSeconds);
			float animStop = animCurve_y->time(animCurve_y->numKeys()-1).as(MTime::kSeconds);		
			if(animStart<_firstStart_cam)
				_firstStart_cam = animStart;
			if(animStop>_lastStop_cam)
				_lastStop_cam = animStop;
			if(animStart<firstStart_cam_local)
				firstStart_cam_local = animStart;
			if(animStop>lastStop_cam_local)
				lastStop_cam_local = animStop;
			m_params.outCameras << "anim " << animCurve_y->name().asChar() << "\n";
			m_params.outCameras <<"\t\t\t{\n";
			m_params.outCameras << "\t\t\t//Time   /    Value\n";		
			for (uint i=0; i<animCurve_y->numKeys(); i++) {
				m_params.outCameras << "\t\t\t" << animCurve_y->time(i).as(MTime::kSeconds) << "\t" << animCurve_y->value(i) << "\n";
				MTime thisTime = animCurve_y->time(i);						
				MTime nextTime = thisTime;
				if( i<animCurve_y->numKeys()-1 )
					nextTime = animCurve_y->time(i+1);
				// find corresponding key in map
				local_it = keyMap.find( thisTime );
				if( local_it != keyMap.end() ) 				// if key already exists, just write the y value and fill up curve til next key
				{
					(local_it->second)[1] = animCurve_y->value(i);
				}
				else											// if not need to insert key with a corresponding value for x
				{
					double *keyVals = new double[3];
					keyVals[0] = animCurve_x->evaluate(thisTime);
					keyVals[1] = animCurve_y->value(i);
					keyVals[2] = 0.0;
					std::pair<std::map<MTime,double*>::iterator,bool> ret;
					ret = keyMap.insert(std::make_pair( animCurve_y->time(i), keyVals) );
				}				
			}
			for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) {						
				(keyMap_it->second)[1] = animCurve_y->evaluate(keyMap_it->first);
			}
			m_params.outCameras << "\t\t}\n\n";
		}
		else
		{
			plug.getValue(dist);
			m_params.outCameras << "= " << dist << "\n";
		}
		//translateZ
		m_params.outCameras << "\t\tz ";
		plug = cameraTransform->findPlug("translateZ");
		if (plug.isConnected() && m_params.exportCamerasAnimXML)
		{
			plug.connectedTo(srcplugarray,true,false,&stat);
			for (int i=0; i< srcplugarray.length(); i++)
			{
				if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
				{
					if (animCurve_z)
						delete animCurve_z;
					animCurve_z = new MFnAnimCurve(srcplugarray[i].node());
					continue;
				}
				else if (i == srcplugarray.length()-1)
				{
					std::cout << "Invalid link to translateZ attribute\n";
					return MS::kFailure;
				}
			}
			float animStart = animCurve_z->time(0).as(MTime::kSeconds);
			float animStop = animCurve_z->time(animCurve_z->numKeys()-1).as(MTime::kSeconds);	
			if(animStart<_firstStart_cam)
				_firstStart_cam = animStart;
			if(animStop>_lastStop_cam)
				_lastStop_cam = animStop;
			if(animStart<firstStart_cam_local)
				firstStart_cam_local = animStart;
			if(animStop>lastStop_cam_local)
				lastStop_cam_local = animStop;
			m_params.outCameras << "anim " << animCurve_z->name().asChar() << "\n";
			m_params.outCameras <<"\t\t\t{\n";
			m_params.outCameras << "\t\t\t//Time   /    Value\n";	
		
			for (uint i=0; i<animCurve_z->numKeys(); i++) {
				m_params.outCameras << "\t\t\t" << animCurve_z->time(i).as(MTime::kSeconds) << "\t" << animCurve_z->value(i) << "\n";
				MTime thisTime = animCurve_z->time(i);
				MTime nextTime = thisTime;
				if( i<animCurve_z->numKeys()-1 )
					nextTime = animCurve_z->time(i+1);
				// find corresponding key in map
				std::map<MTime, double* >::iterator local_it;
				local_it = keyMap.find( thisTime );
				if( local_it != keyMap.end() ) 				// if key already exists, just write the z value and fill up curve til next key
				{
					(local_it->second)[2] = animCurve_z->value(i);					
				}
				else											// if not need to insert key with a corresponding value for x
				{
					double *keyVals = new double[3];
					keyVals[0] = animCurve_x->evaluate(thisTime);
					keyVals[1] = animCurve_y->evaluate(thisTime);
					keyVals[2] = animCurve_z->value(i);
					std::pair<std::map<MTime,double*>::iterator,bool> ret;
					ret = keyMap.insert(std::make_pair( animCurve_z->time(i), keyVals) );
				}
			}
			for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) {						
				(keyMap_it->second)[2] = animCurve_z->evaluate(keyMap_it->first);
			}
			m_params.outCameras << "\t\t}\n\n";
		}
		else
		{
			plug.getValue(dist);
			m_params.outCameras << "= " << dist << "\n";
		}
		m_params.outCameras << "\t}\n";
		outCamerasXML_tmp << "\t<animation name=\"translate\" type=\"float3\" start=\"" << firstStart_cam_local  << "\" end=\"" << lastStop_cam_local << "\">\n" ;
		for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) 
			outCamerasXML_tmp << "\t\t<keyframe time=\"" << keyMap_it->first.as(MTime::kSeconds) << "\" value=\"(" << (keyMap_it->second)[0] << "," << (keyMap_it->second)[1] << "," << (keyMap_it->second)[2] << ")\"/>\n";
		outCamerasXML_tmp << "\t</animation>\n";
		for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) {
			double* toBeDestroyed = keyMap_it->second;
			delete toBeDestroyed;
			keyMap_it->second = NULL;
		}
		keyMap.clear();
		firstStart_cam_local = 1e12;
		lastStop_cam_local = -1.;
		// write rotation data
		m_params.outCameras << "\trotation\n";
		m_params.outCameras << "\t{\n";
		m_params.outCameras << "\t\tx ";
		//rotateX
		plug = cameraTransform->findPlug("rotateX");
		if (plug.isConnected() && m_params.exportCamerasAnimXML)
		{
			plug.connectedTo(srcplugarray,true,false,&stat);
			for (int i=0; i < srcplugarray.length(); i++)
			{
				if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
				{
					if (animCurve_x)
						delete animCurve_x;
					animCurve_x = new MFnAnimCurve(srcplugarray[i].node());
					continue;
				}
				else if (i == srcplugarray.length()-1)
				{
					std::cout << "Invalid link to rotateX attribute\n";
					return MS::kFailure;
				}
			}
			float animStart = animCurve_x->time(0).as(MTime::kSeconds);
			float animStop = animCurve_x->time(animCurve_x->numKeys()-1).as(MTime::kSeconds);		
			if(animStart<_firstStart_cam)
				_firstStart_cam = animStart;
			if(animStop>_lastStop_cam)
				_lastStop_cam = animStop;
			if(animStart<firstStart_cam_local)
				firstStart_cam_local = animStart;
			if(animStop>lastStop_cam_local)
				lastStop_cam_local = animStop;
			m_params.outCameras << "anim " << animCurve_x->name().asChar() << "\n";
			m_params.outCameras <<"\t\t\t{\n";
			m_params.outCameras << "\t\t\t//Time   /    Value\n";
			for (uint i=0; i<animCurve_x->numKeys(); i++) {
				m_params.outCameras << "\t\t\t" << animCurve_x->time(i).as(MTime::kSeconds) << "\t" << animCurve_x->value(i) << "\n";
				// x is reference for the anim map
				double *keyVals = new double[3];
				keyVals[0] = animCurve_x->value(i);
				keyVals[1] = 0.0;
				keyVals[2] = 0.0;
				keyMap.insert(std::make_pair( animCurve_x->time(i), keyVals) );
			}
			m_params.outCameras << "\t\t}\n\n";
		}
		else
		{
			plug.getValue(angle);
			m_params.outCameras << "= " << angle.asDegrees() << "\n";
		}
		//rotateY
		m_params.outCameras << "\t\ty ";
		plug = cameraTransform->findPlug("rotateY");
		if (plug.isConnected() && m_params.exportCamerasAnimXML)
		{
			plug.connectedTo(srcplugarray,true,false,&stat);
			for (int i=0; i< srcplugarray.length(); i++)
			{
				if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
				{
					if (animCurve_y)
						delete animCurve_y;
					animCurve_y = new MFnAnimCurve(srcplugarray[i].node());
					continue;
				}
				else if (i == srcplugarray.length()-1)
				{
					std::cout << "Invalid link to translateY attribute\n";
					return MS::kFailure;
				}
			}
			float animStart = animCurve_y->time(0).as(MTime::kSeconds);
			float animStop = animCurve_y->time(animCurve_y->numKeys()-1).as(MTime::kSeconds);		
			if(animStart<_firstStart_cam)
				_firstStart_cam = animStart;
			if(animStop>_lastStop_cam)
				_lastStop_cam = animStop;
			if(animStart<firstStart_cam_local)
				firstStart_cam_local = animStart;
			if(animStop>lastStop_cam_local)
				lastStop_cam_local = animStop;
			m_params.outCameras << "anim " << animCurve_y->name().asChar() << "\n";
			m_params.outCameras <<"\t\t\t{\n";
			m_params.outCameras << "\t\t\t//Time   /    Value\n";		
			for (uint i=0; i<animCurve_y->numKeys(); i++) {
				m_params.outCameras << "\t\t\t" << animCurve_y->time(i).as(MTime::kSeconds) << "\t" << animCurve_y->value(i) << "\n";
				MTime thisTime = animCurve_y->time(i);						
				MTime nextTime = thisTime;
				if( i<animCurve_y->numKeys()-1 )
					nextTime = animCurve_y->time(i+1);
				// find corresponding key in map
				local_it = keyMap.find( thisTime );
				if( local_it != keyMap.end() ) 				// if key already exists, just write the y value and fill up curve til next key
				{
					(local_it->second)[1] = animCurve_y->value(i);
				}
				else											// if not need to insert key with a corresponding value for x
				{
					double *keyVals = new double[3];
					keyVals[0] = animCurve_x->evaluate(thisTime);
					keyVals[1] = animCurve_y->value(i);
					keyVals[2] = 0.0;
					std::pair<std::map<MTime,double*>::iterator,bool> ret;
					ret = keyMap.insert(std::make_pair( animCurve_y->time(i), keyVals) );
				}				
			}
			for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) {						
				(keyMap_it->second)[1] = animCurve_y->evaluate(keyMap_it->first);
			}
			m_params.outCameras << "\t\t}\n\n";
		}	
		else
		{
			plug.getValue(angle);
			m_params.outCameras << "= " << angle.asDegrees() << "\n";
		}
		//rotateZ
		m_params.outCameras << "\t\tz ";
		plug = cameraTransform->findPlug("rotateZ");
		if (plug.isConnected() && m_params.exportCamerasAnimXML)
		{
			plug.connectedTo(srcplugarray,true,false,&stat);
			for (int i=0; i< srcplugarray.length(); i++)
			{
				if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
				{
					if (animCurve_z)
						delete animCurve_z;
					animCurve_z = new MFnAnimCurve(srcplugarray[i].node());
					continue;
				}
				else if (i == srcplugarray.length()-1)
				{
					std::cout << "Invalid link to translateZ attribute\n";
					return MS::kFailure;
				}
			}
			float animStart = animCurve_z->time(0).as(MTime::kSeconds);
			float animStop = animCurve_z->time(animCurve_z->numKeys()-1).as(MTime::kSeconds);	
			if(animStart<_firstStart_cam)
				_firstStart_cam = animStart;
			if(animStop>_lastStop_cam)
				_lastStop_cam = animStop;
			if(animStart<firstStart_cam_local)
				firstStart_cam_local = animStart;
			if(animStop>lastStop_cam_local)
				lastStop_cam_local = animStop;
			m_params.outCameras << "anim " << animCurve_z->name().asChar() << "\n";
			m_params.outCameras <<"\t\t\t{\n";
			m_params.outCameras << "\t\t\t//Time   /    Value\n";	
		
			for (uint i=0; i<animCurve_z->numKeys(); i++) {
				m_params.outCameras << "\t\t\t" << animCurve_z->time(i).as(MTime::kSeconds) << "\t" << animCurve_z->value(i) << "\n";
				MTime thisTime = animCurve_z->time(i);
				MTime nextTime = thisTime;
				if( i<animCurve_z->numKeys()-1 )
					nextTime = animCurve_z->time(i+1);
				// find corresponding key in map
				std::map<MTime, double* >::iterator local_it;
				local_it = keyMap.find( thisTime );
				if( local_it != keyMap.end() ) 				// if key already exists, just write the z value and fill up curve til next key
				{
					(local_it->second)[2] = animCurve_z->value(i);					
				}
				else											// if not need to insert key with a corresponding value for x
				{
					double *keyVals = new double[3];
					keyVals[0] = animCurve_x->evaluate(thisTime);
					keyVals[1] = animCurve_y->evaluate(thisTime);
					keyVals[2] = animCurve_z->value(i);
					std::pair<std::map<MTime,double*>::iterator,bool> ret;
					ret = keyMap.insert(std::make_pair( animCurve_z->time(i), keyVals) );
				}
			}
			for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) {						
				(keyMap_it->second)[2] = animCurve_z->evaluate(keyMap_it->first);
			}
			m_params.outCameras << "\t\t}\n\n";
		}
		else
		{
			plug.getValue(angle);
			m_params.outCameras << "= " << angle.asDegrees() << "\n";
		}
		m_params.outCameras << "\t}\n";
		outCamerasXML_tmp << "\t<animation name=\"rotate\" type=\"float3\" start=\"" << firstStart_cam_local  << "\" end=\"" << lastStop_cam_local << "\">\n" ;
		for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) 
			outCamerasXML_tmp << "\t\t<keyframe time=\"" << keyMap_it->first.as(MTime::kSeconds) << "\" value=\"(" << (keyMap_it->second)[0] << "," << (keyMap_it->second)[1] << "," << (keyMap_it->second)[2] << ")\"/>\n";
		outCamerasXML_tmp << "\t</animation>\n";
		for(keyMap_it=keyMap.begin();keyMap_it!=keyMap.end();keyMap_it++) {
			double* toBeDestroyed = keyMap_it->second;
			delete toBeDestroyed;
			keyMap_it->second = NULL;
		}
		keyMap.clear();
		firstStart_cam_local = 1e12;
		lastStop_cam_local = -1.;
		// export of FOVy
		plug = camera.findPlug("verticalFilmAperture");
		if (plug.isConnected() && m_params.exportCamerasAnimXML)
		{
			plug.connectedTo(srcplugarray,true,false,&stat);
			for (int i=0; i< srcplugarray.length(); i++)
			{
				if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
				{
					if (animCurve)
						delete animCurve;
					animCurve = new MFnAnimCurve(srcplugarray[i].node());
					continue;
				}
				else if (i == srcplugarray.length()-1)
				{
					std::cout << "Invalid link to verticalFilmAperture attribute\n";
					return MS::kFailure;
				}
			}
			float animStart = animCurve->time(0).as(MTime::kSeconds);
			float animStop = animCurve->time(animCurve->numKeys()-1).as(MTime::kSeconds);
			if(animStart<_firstStart_cam)
				_firstStart_cam = animStart;
			if(animStop>_lastStop_cam)
				_lastStop_cam = animStop;
			if(animStart<firstStart_cam_local)
				firstStart_cam_local = animStart;
			if(animStop>lastStop_cam_local)
				lastStop_cam_local = animStop;
			if(_cam_vA_map.size() > 0)
				_cam_vA_map.clear();
			for (uint i=0; i<animCurve->numKeys(); i++) 
				_vA_map.insert(std::make_pair(animCurve->time(i).as(MTime::kSeconds), animCurve->value(i)));			
		
			plug = camera.findPlug("focalLength");
			if (plug.isConnected() && m_params.exportCamerasAnimXML)
			{
				plug.connectedTo(srcplugarray,true,false,&stat);
				for (int i=0; i< srcplugarray.length(); i++)
				{
					if (srcplugarray[i].node().hasFn(MFn::kAnimCurve))
					{
						if (animCurve)
							delete animCurve;
						animCurve = new MFnAnimCurve(srcplugarray[i].node());
						continue;
					}
					else if (i == srcplugarray.length()-1)
					{
						std::cout << "Invalid link to focalLength attribute\n";
						return MS::kFailure;
					}
				}
				std::map<float,float>::iterator av_it;
				m_params.outCameras << "anim FOVy\n";
				m_params.outCameras <<"\t\t\t{\n";
				m_params.outCameras << "\t\t\t//Time   /    Value\n";
				outCamerasXML_tmp << "\t<animation name=\"FOVy\" type=\"float\" start=\"" <<  animStart << "\" end=\"" << animStop << "\">\n" ;
				if(animCurve->numKeys()>_vA_map.size()) {			
							// TODO
					}				
				else {
					for(av_it=_vA_map.begin();av_it!=_vA_map.end();av_it++) {			
						float last_key = 0.0;
						float take_this_key = -1.0;
						for(uint i=0; i<animCurve->numKeys(); i++) {
							if(abs(animCurve->time(i).as(MTime::kSeconds) - av_it->first)<0.0001) {
								take_this_key = animCurve->value(i);
								break;
							}
							else {
								last_key = animCurve->value(i);
							}
						}
						float vA_val = av_it->second;
						float focalLength = -1. ; 
						if(take_this_key > -1.1) 
							focalLength = take_this_key;
						else
							focalLength = last_key;
						float FOVy = 2*atan(vA_val*25.4/(2*focalLength)) * 180 / 3.141592654; 
						outCamerasXML_tmp << "\t\t<keyframe time=\"" << av_it->first << "\" value=\"" << FOVy << "\"/>\n";
					}				
				}
			outCamerasXML_tmp << "\t</animation>\n";
			}
			else {
					cout << "Focal length is not connected -> FOVy will not be exported" << endl;
			}			
		}
		else {
			cout << "Vertical aperture is not connected -> FOVy will not be exported" << endl;
		}
		// end camera description
		m_params.outCameras << "}\n\n";
		//write camera type
		m_params.outCameras << "\ttype ";
		if (camera.isOrtho()) {
			m_params.outCameras << "ortho\n";
			m_params.outCamerasXML << "<camera name=\"" << cameraTransform->partialPathName().asChar() << "\" type=\"ortho\" start=\"" << _firstStart_cam << "\" end=\"" << _lastStop_cam << "\">\n";
		}	
		else {
			m_params.outCameras << "persp\n";
			m_params.outCamerasXML << "<camera name=\"" << cameraTransform->partialPathName().asChar() << "\" type=\"persp\" start=\"" << _firstStart_cam << "\" end=" << _lastStop_cam << "\">\n";
		}
		std::string s;
		outCamerasXML_tmp.seekg (0, ios::beg);
	    while (!outCamerasXML_tmp.eof())         
		{
			std::getline(outCamerasXML_tmp, s);  
			if(s!="" )
				m_params.outCamerasXML << s << "\n";    
		}
		
		m_params.outCamerasXML << "</camera>\n" ;
		m_params.outCamerasXML << "\n" ;
		if (cameraTransform != NULL)
			delete cameraTransform;
		if (animCurve != NULL)
			delete animCurve;
		return MS::kSuccess;
	}
	/********************************************************************************************************
	*                           Method to write data to OGRE format                                         *
	********************************************************************************************************/
	MStatus OgreExporter::writeOgreData()
	{
		// Create singletons		
		Ogre::LogManager logMgr;
		Ogre::ResourceGroupManager rgm;
		Ogre::MeshManager meshMgr;
		Ogre::SkeletonManager skelMgr;
		Ogre::MaterialManager matMgr;
		Ogre::DefaultHardwareBufferManager hardwareBufMgr;
		Ogre::LodStrategyManager lodMgr;
		
		// Create a log
		logMgr.createLog("ogreMayaExporter.log", true);
		// Write mesh binary
		if (m_params.exportMesh)
		{
			std::cout << "Writing mesh binary...\n";
			std::cout.flush();
			stat = m_pMesh->writeOgreBinary(m_params);
			if (stat != MS::kSuccess)
			{
				std::cout << "Error writing mesh binary file\n";
				std::cout.flush();
			}
		}
		// Write skeleton binary
		if (m_params.exportSkeleton)
		{
			if (m_pMesh->getSkeleton())
			{
				std::cout << "Writing skeleton binary...\n";
				std::cout.flush();
				stat = m_pMesh->getSkeleton()->writeOgreBinary(m_params);
				if (stat != MS::kSuccess)
				{
					std::cout << "Error writing mesh binary file\n";
					std::cout.flush();
				}
			}
		}
		
		// Write materials data
		if (m_params.exportMaterial)
		{
			std::cout << "Writing materials data...\n";
			std::cout.flush();
			stat  = m_pMaterialSet->writeOgreScript(m_params);
			if (stat != MS::kSuccess)
			{
				std::cout << "Error writing materials file\n";
				std::cout.flush();
			}
		}
		return MS::kSuccess;
	}
	
	 MStatus OgreExporter::writeClipLengths(ParamList& params)
	 {		 
		 if (m_params.exportMesh && m_params.exportVertAnims)
			{
				std::cout << "Sending clip lengths to anim file...\n";
				std::cout.flush();
				//m_vertexClips.clear();
				// load the requested clips
				MStatus status = MS::kSuccess;
				if(params.vertClipList.size() == 0)
					stat = MS::kFailure;
				for (int i=0; i<params.vertClipList.size(); i++)
				{
					MString clipName =  params.vertClipList[i].name;
					std::cout << "Checking length for clip " << clipName.asChar() << "\n";
					std::cout.flush();
					float start = params.vertClipList[i].start;
					float stop = params.vertClipList[i].stop;
					float rate = params.vertClipList[i].rate;
					std::vector<float> times;
					// calculate times from clip sample rate
					times.clear();
					for (float t=start; t<stop; t+=rate)
						times.push_back(t);
					times.push_back(stop);
					// get animation length
					float length=0;
					if (times.size() >= 0)
						length = times[times.size()-1] - times[0];
					// write first clip to anim file
					if(i == 0)
					{
						m_params.outAnim << "anim Clip_Length\n";
						m_params.outAnim <<"{\n";
						m_params.outAnim << "\t" << length << " 0 \n";
						m_params.outAnim << "}\n\n";
						m_params.outAnim << "anim Clip_Start\n";
						m_params.outAnim <<"{\n";
						m_params.outAnim << "\t" << start << " 0 \n";
						m_params.outAnim << "}\n\n";
						m_params.outAnim << "anim Clip_Stop\n";
						m_params.outAnim <<"{\n";
						m_params.outAnim << "\t" << stop << " 0 \n";
						m_params.outAnim << "}\n\n";
						m_params.outAnim << "anim Clip_Values\n";
						m_params.outAnim <<"{\n";
						m_params.outAnim << "\t//Time   /    Value\n";
						std::vector<float>::iterator timeIt;
						for(timeIt = times.begin(); timeIt!=times.end();timeIt++)
						{
							m_params.outAnim << "\t" << *timeIt << "\t" << (((*timeIt)-times[0])/length*100) << "\n";
						}
						m_params.outAnim << "}\n\n";
					}
				}
				if (stat != MS::kSuccess)
				{
					std::cout << "Error sending clip lengths to anim file\n";
					std::cout.flush();
					return MS::kFailure;
				}
			}		
		else if (m_params.exportAnimCurves)
		{
			std::cout << "Length export from anim curves.\n";
			m_params.outAnim << "anim Clip_Length\n";
			m_params.outAnim <<"{\n";
			m_params.outAnim << "\t" << _lastStop-_firstStart << " 0 \n";
			m_params.outAnim << "}\n\n";
			m_params.outAnim << "anim Clip_Start\n";
			m_params.outAnim <<"{\n";
			m_params.outAnim << "\t" << _firstStart << " 0 \n";
			m_params.outAnim << "}\n\n";
			m_params.outAnim << "anim Clip_Stop\n";
			m_params.outAnim <<"{\n";
			m_params.outAnim << "\t" << _lastStop << " 0 \n";
			m_params.outAnim << "}\n\n";
			std::cout.flush();
		}
		return MS::kSuccess;
	 }
} // end namespace

// Routine for registering the command within Maya
MStatus initializePlugin( MObject obj )
{
	MStatus   status;
	MFnPlugin plugin( obj, "OgreExporter", "v1.0.0-github", "Any");
	status = plugin.registerCommand( "ogreExport", OgreMayaExporter::OgreExporter::creator );
	if (!status) {
		status.perror("registerCommand");
		return status;
	}
	
	return status;
}

// Routine for unregistering the command within Maya
MStatus uninitializePlugin( MObject obj)
{
	MStatus   status;
	MFnPlugin plugin( obj );
	status = plugin.deregisterCommand( "ogreExport" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
	
	return status;
}
