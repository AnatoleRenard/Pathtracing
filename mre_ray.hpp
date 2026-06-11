#pragma once
#include <glm/glm.hpp>

namespace mre
{
	class MreRay
	{
		public:
			MreRay(const glm::dvec3& origin, const glm::dvec3& direction) : origin(origin), direction(direction), invDir(1.0/direction){}
			MreRay(){}
			~MreRay(){}
			
			glm::dvec3 at(double t) const
			{
				return origin + direction * t;
			}
			
			glm::dvec3 origin;
			glm::dvec3 direction;
			glm::dvec3 invDir;
	};
}
