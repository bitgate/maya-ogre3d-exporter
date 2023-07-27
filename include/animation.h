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
//! \file "animation.h"
//! \brief Animation header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef ANIMATION_H
#define ANIMATION_H

#include "mayaExportLayer.h"

namespace OgreMayaExporter
{
	// Track type
	typedef enum { TT_SKELETON, TT_MORPH, TT_POSE } trackType;

	// Target
	typedef enum { T_MESH, T_SUBMESH } target;

	// Vertex position
	typedef struct vertexPositiontag
	{
		float x,y,z;
	} vertexPosition;

	// Vertex pose reference
	typedef struct vertexPoseReftag
	{
		int poseIndex;
		float poseWeight;
	} vertexPoseRef;

	// Vertex animation keyframe
	typedef struct vertexKeyframetag
	{
		float time;
		std::vector<vertexPosition> positions;
		std::vector<vertexPoseRef> poserefs;
	} vertexKeyframe;

	// Skeleton animation keyframe
	typedef struct skeletonKeyframeTag
	{
		float time;								//time of keyframe
		double tx,ty,tz;						//translation
		double angle,axis_x,axis_y,axis_z;		//rotation
		float sx,sy,sz;							//scale
	} skeletonKeyframe;

	// Blend shape data
	typedef struct vertexOffestTag
	{
		long index;
		float x,y,z;
	} vertexOffset;

	typedef struct poseTag
	{
		target poseTarget;
		long index;
		int blendShapeIndex;
		MString name;
		std::vector<vertexOffset> offsets;
	} pose;

	// A class for storing an animation track
	// each track can be either skeleton, morph or pose animation
	class Track
	{
	public:
		//constructor
		Track() {
			clear();
		};
		//destructor
		~Track() {
			clear();
		}
		//clear track data
		void clear() {
			m_type = TT_SKELETON;
			m_target = T_MESH;
			m_index = 0;
			m_bone = "";
			m_vertexKeyframes.clear();
			m_skeletonKeyframes.clear();
		}
		//add vertex animation keyframe
		void addVertexKeyframe(vertexKeyframe& k) {
			m_vertexKeyframes.push_back(k);
		}
		//add skeleton animation keyframe
		void addSkeletonKeyframe(skeletonKeyframe& k) {
			m_skeletonKeyframes.push_back(k);
		}

		//public members
		trackType m_type;
		target m_target;
		int m_index;
		MString m_bone;
		std::vector<vertexKeyframe> m_vertexKeyframes;
		std::vector<skeletonKeyframe> m_skeletonKeyframes;
	};
	

	// A class for storing animation information
	// an animation is a collection of different tracks
	class Animation
	{
	public:
		//constructor
		Animation() {
			clear();
		}
		//destructor
		~Animation() { 
			clear(); 
		};
		//clear animation data
		void clear() {
			m_name = "";
			m_length = 0;
			m_tracks.clear();
		};
		//add track
		void addTrack(Track& t) {
			m_tracks.push_back(t);
		}

		//public memebers
		MString m_name;
		float m_length;
		std::vector<Track> m_tracks;
	};

} // end namespace


#endif // ANIMATION_H