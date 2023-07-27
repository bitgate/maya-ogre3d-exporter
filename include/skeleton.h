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
//! \file "skeleton.h"
//! \brief Skeleton header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef _SKELETON_H
#define _SKELETON_H

#include "mayaExportLayer.h"
#include "paramList.h"
#include "animation.h"

namespace OgreMayaExporter
{
	/***** structure to hold joint info *****/
	typedef struct jointTag
	{
		MString name;
		int id;
		MMatrix localMatrix;
		MMatrix bindMatrix;
		int parentIndex;
		double posx,posy,posz;
		double angle;
		double axisx,axisy,axisz;
		float scalex,scaley,scalez;
		MDagPath jointDag;
	} joint;


	/*********** Class Skeleton **********************/
	class Skeleton
	{
	public:
		//constructor
		Skeleton();
		//destructor
		~Skeleton();
		//clear skeleton data
		void clear();
		//load skeleton data
		MStatus load(MFnSkinCluster* pSkinCluster,ParamList& params);
		//load skeletal animations
		MStatus loadAnims(ParamList& params);
		//get joints
		std::vector<joint>& getJoints();
		//get animations
		std::vector<Animation>& getAnimations();
		//restore skeleton pose
		void restorePose();
		//write to an OGRE binary skeleton
		MStatus writeOgreBinary(ParamList &params);
		// test for shear(non-uniform scale)
		void Skeleton::testShear( MString&, MMatrix&, const std::string& );

	protected:
		//load a joint
		MStatus loadJoint(MDagPath& jointDag, joint* parent, ParamList& params,MFnSkinCluster* pSkinCluster);
		//load a clip
		MStatus loadClip(MString clipName,float start,float stop,float rate,ParamList& params);
		//load a keyframe for a particular joint at current time
		skeletonKeyframe loadKeyframe(joint& j,float time,ParamList& params);
		//write joints to an Ogre skeleton
		MStatus createOgreBones(Ogre::SkeletonPtr pSkeleton,ParamList& params);
		// write skeleton animations to an Ogre skeleton
		MStatus createOgreSkeletonAnimations(Ogre::SkeletonPtr pSkeleton,ParamList& params);
		
		std::vector<joint> m_joints;
		std::vector<Animation> m_animations;
		std::vector<int> m_roots;
		MString m_restorePose;
	};

}	//end namespace

#endif
