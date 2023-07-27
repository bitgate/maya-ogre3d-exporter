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
//! \file "material.h"
//! \brief material header for Ogre Exporter.
//!
//! \author     Francesco Giordana <fra.giordana@tiscali.it>
//! \author     Marcel Reinhard <marcel.reinhardt@filmakademie.de>
//! \version    1.0
//! \date       21.10.2010 (last updated)
//!

#ifndef _MATERIAL_H
#define _MATERIAL_H

#include "mayaExportLayer.h"
#include "paramList.h"

namespace OgreMayaExporter
{

	typedef enum {MT_SURFACE_SHADER,MT_LAMBERT,MT_PHONG,MT_BLINN,MT_CGFX} MaterialType;

	typedef enum {TOT_REPLACE,TOT_MODULATE,TOT_ADD,TOT_ALPHABLEND} TexOpType;

	typedef enum {TAM_CLAMP,TAM_BORDER,TAM_WRAP,TAM_MIRROR} TexAddressMode;

	class Texture
	{
	public:
		//constructor
		Texture() {
			scale_u = scale_v = 1;
			scroll_u = scroll_v = 0;
			rot = 0;
			am_u = am_v = TAM_CLAMP;
		}
		//destructor
		~Texture(){};
	
		//public members
		MString filename;
		MString absFilename;
		TexOpType opType;
		MString uvsetName;
		int uvsetIndex;
		TexAddressMode am_u,am_v;
		double scale_u,scale_v;
		double scroll_u,scroll_v;
		double rot;
	};


	/***** Class Material *****/
	class Material
	{
	public:
		//constructor
		Material();
		//destructor
		~Material();
		//get material name
		MString& name();
		//clear material data
		void clear();
		//load material data
		MStatus load(MFnDependencyNode* pShader,MStringArray& uvsets,ParamList& params);
		//load a specific material type
		MStatus loadSurfaceShader(MFnDependencyNode* pShader);
		MStatus loadLambert(MFnDependencyNode* pShader);
		MStatus loadPhong(MFnDependencyNode* pShader);
		MStatus loadBlinn(MFnDependencyNode* pShader);
		MStatus loadCgFxShader(MFnDependencyNode* pShader);
		//write material data to Ogre material script
		MStatus writeOgreScript(ParamList &params);
		//copy textures to path specified by params
		MStatus copyTextures(ParamList &params);
	public:
		//load texture data
		MStatus loadTexture(MFnDependencyNode* pTexNode,TexOpType& opType,MStringArray& uvsets,ParamList& params);

		MString m_name;
		MaterialType m_type;
		MColor m_ambient, m_diffuse, m_specular, m_emissive;
		bool m_lightingOff;
		bool m_isTransparent;
		bool m_isTextured;
		bool m_isMultiTextured;
		std::vector<Texture> m_textures;
	};

};	//end of namespace

#endif