#pragma once
#include <string>
#include <memory>
#include <iostream>
#include "mre_aabb.hpp"

//assimp library for object loading
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//glm
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace mre
{
	
	const int FRONT = 1;
	const int BACK  = -1;
	const int BOTH  = 0;
	
	struct MreTransformModel
	{
		glm::dvec3 scale    = glm::dvec3(0.0); // multiply
		glm::dvec3 rotation = glm::dvec3(0.0); //add
		glm::dvec3 position = glm::dvec3(0.0); //add
	};
	
	class MreModel : public MreObject
	{
		public:
			MreModel(std::string file, std::shared_ptr<MreMaterial> mat, const MreTransformModel& transform = MreTransformModel(), int sidesToSee = 0, int minTriInBVH = 10);
			
			void hit(const MreRay& ray, MreHitRec& rec) override
			{
				//when using bvh
				if(bvh.hit(ray, rec) < infinity)
				{
					bvh.hitKids(ray, rec);
				}
				
				/*//when not using bvh
				for(const auto& tri : triangles)
				{
					tri->hit(ray, rec);
				}*/
			}
			
			void add(std::shared_ptr<MreObject> obj) override{}
			
			std::vector<std::shared_ptr<MreTriangle>> triangles;
			MreAABB bvh;
	};
}
