#pragma once
#include <glm/glm.hpp>
#include "mre_utility.hpp"

namespace mre
{
	class MreMaterial;
	
	class MreHitRec
	{
		public:
		
			bool hit = false;
			glm::dvec3 normal;
			glm::dvec3 point;
			double at = infinity;
			double min = epsilonCam;
			bool frontFace;
			double u;
			double v;
			std::shared_ptr<MreMaterial> mat;
			
			void setNormal(const MreRay& r, const glm::dvec3& outward_normal)
			{
				// Sets the hit record normal vector.
				// NOTE: the parameter `outward_normal` is assumed to have unit length.

				frontFace = glm::dot(r.direction, outward_normal) < epsilon;
				normal = frontFace ? outward_normal : -outward_normal;
			}
	};
}
