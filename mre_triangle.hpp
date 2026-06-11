#pragma once
#include "mre_object.hpp"

namespace mre
{
	class MreTriangle : public MreObject
	{
		public:
			//seeSides = 0 -> both sides, seeSides => 1 -> front only, seeSides <= -1 back only
			MreTriangle(const glm::dvec3& posA, const glm::dvec3& posB, const glm::dvec3& posC,
						const glm::dvec3& norA, const glm::dvec3& norB, const glm::dvec3& norC,
						std::shared_ptr<MreMaterial> mat, int seeSides = 0) : posA(posA), posB(posB), posC(posC), norA(norA), norB(norB), norC(norC), mat(mat), 
						center((posA+posB+posC)/3.0), seeBack(seeSides <= 0 ? true : false), seeFront(seeSides >= 0 ? true : false) {}
				
			void hit(const MreRay& ray, MreHitRec& rec) override
			{
				glm::dvec3 edgeAB = posB - posA;
				glm::dvec3 edgeAC = posC - posA;
				glm::dvec3 normalVector = glm::cross(edgeAB, edgeAC);
				glm::dvec3 ao = ray.origin - posA;
				glm::dvec3 dao = glm::cross(ao, ray.direction);
				
				//check side
				glm::dvec3 normalizedNormal = glm::normalize(normalVector);
				auto angle = glm::dot(normalizedNormal, ray.direction);
				
				//if hit back of plane
				if(!seeBack && angle > epsilonCam)
				{
					return;
				}
				
				//if hit front of plane
				if(!seeFront && angle < epsilonCam)
				{
					return;
				}
				
				double determinant = -glm::dot(ray.direction, normalVector);
				double invDet = 1.0 / determinant;
				
				//Calculate distance to triangle & barycentric coordinates of intersection points
				double dst = glm::dot(ao, normalVector) * invDet;
				double u = glm::dot(edgeAC, dao) * invDet;
				double v = -glm::dot(edgeAB, dao) * invDet;
				double w = 1 - u - v;
				
				if(fabs(determinant) >= epsilon && dst >= rec.min && u >= epsilon && v >= epsilon && w >= epsilon && rec.at > dst)
				{
					rec.hit = true;
					rec.at = dst;
					rec.point = ray.at(dst);
					rec.setNormal(ray, glm::normalize(norA*w + norB * u + norC * v));
					rec.u = u;
					rec.v = v;
					rec.mat = mat;
				}
			}
			
			void add(std::shared_ptr<MreObject> obj) override{}
			
			glm::dvec3 center;
			glm::dvec3 posA;
			glm::dvec3 posB;
			glm::dvec3 posC;
			
		private:
			bool seeBack;
			bool seeFront;
			glm::dvec3 norA;
			glm::dvec3 norB;
			glm::dvec3 norC;
			
			std::shared_ptr<MreMaterial> mat;
	};
}
