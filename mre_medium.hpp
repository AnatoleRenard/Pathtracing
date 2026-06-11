#pragma once
#include "mre_object.hpp"
#include "mre_material.hpp"

#include <cmath>

namespace mre
{
	class MreMedium : public MreObject
	{
		public:
			MreMedium(const std::shared_ptr<MreObject> obj, const std::shared_ptr<MreTexture> tex, double density) :
			obj(obj), mat(std::make_shared<MreIsotropic>(tex)), negativeInverseDensity(-1.0/density){}
			
			void hit(const MreRay& ray, MreHitRec& rec) override
			{
				//check if hits obj twice
				MreHitRec rec1, rec2;
				rec1.at = rec.at;
				rec1.min = rec.min;
				obj->hit(ray, rec1);
				if(!rec1.hit)
					return;
				
				rec2.min = rec1.at + epsilonCam;
				rec2.at = rec.at;
				obj->hit(ray, rec2);
				if(!rec2.hit)
					return;
				
				double rayLength = glm::length(ray.direction);
				double distance = (rec2.at - rec1.at) * rayLength;
				double hitDist = negativeInverseDensity * log(randomDouble());
				
				if(hitDist > distance)
					return;
				
				rec.hit = true;
				rec.at = rec1.at + hitDist / rayLength;
				rec.point = ray.at(rec.at);
				
				rec.normal = glm::dvec3(1,0,0);  // arbitrary
				rec.frontFace = true;            // also arbitrary
				rec.mat = mat;
			}
			
			void add(std::shared_ptr<MreObject> obj) override{}
			
		private:
			std::shared_ptr<MreObject> obj;
			double negativeInverseDensity;
			std::shared_ptr<MreMaterial> mat;
	};
}
