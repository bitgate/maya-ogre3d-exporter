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
//! \file "vertex.h"
//! \brief Vertex header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef _VERTEX_H
#define _VERTEX_H

/***** structure for uvsets info *****/
	typedef struct uvsettag
	{
		short size;					//number of coordinates (between 1 and 3)
	} uvset;
	/***** structure for texture coordinates *****/
	typedef struct texcoordstag
	{
		float u, v, w;				//texture coordinates	
	} texcoord;

	/***** structure for vertex bone assignements *****/
	typedef struct vbatag
	{
		float weight;	//weight
		int jointIdx;	//index of associated joint
	} vba;

	/***** structure for vertex data *****/
	typedef struct vertextag
	{
		double x, y, z;						//vertex coordinates
		MVector n;							//vertex normal
		float r,g,b,a;						//vertex colour
		std::vector<texcoord> texcoords;	//vertex texture coordinates
		std::vector<vba> vbas;				//vertex bone assignements
		long index;							//vertex index in the maya mesh to which this vertex refers
	} vertex;

	/***** structure for vertex info *****/
	// used to hold indices to access MFnMesh data
	typedef struct vertexInfotag
	{
		int pointIdx;				//index to points list (position)
		int normalIdx;				//index to normals list
		float r,g,b,a;				//colour
		std::vector<float> u;		//u texture coordinates
		std::vector<float> v;		//v texture coordinates
		std::vector<float> vba;		//vertex bone assignements
		std::vector<int> jointIds;	//ids of joints affecting this vertex
		int next;					//index of next vertex with same position
	} vertexInfo;

	/***** structure for face info *****/
	typedef struct facetag
	{
		long v[3];		//vertex indices
	} face;

	/***** array of face infos *****/
	typedef std::vector<face> faceArray;

#endif