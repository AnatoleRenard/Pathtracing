#pragma once
#include "mre_object.hpp"


namespace mre
{
	class MreQuad : public MreObject
		{
		public:
			MreQuad(const glm::dvec3& _Q, const glm::dvec3& _u, const glm::dvec3& _v, std::shared_ptr<MreMaterial> mat,
					bool seeBack = true, bool seeFront = true) : Q(_Q), u(_u), v(_v), mat(mat), seeBack(seeBack), seeFront(seeFront)
			{
				auto n = cross(u, v);
				normal = glm::normalize(n);
				D = glm::dot(normal, Q);
				w = n / glm::dot(n,n);
			}
			
			void hit(const MreRay& ray, MreHitRec& rec) override
			{
				auto denom = glm::dot(normal, ray.direction);

				//if hit back of plane
				if(!seeBack && denom > epsilonCam)
				{
					return;
				}
				
				//if hit front of plane
				if(!seeFront && denom < epsilonCam)
				{
					return;
				}
				
				// No hit if the ray is parallel to the plane.
				if (fabs(denom) < epsilonCam)
					return;

				// Return false if the hit point parameter t is outside the ray interval.
				auto t = (D - glm::dot(normal, ray.origin)) / denom;
				if (t < rec.min || t > rec.at)
					return;

				// Determine the hit point lies within the planar shape using its plane coordinates.
				auto intersection = ray.at(t);
				glm::dvec3 planar_hitpt_vector = intersection - Q;
				auto alpha = glm::dot(w, glm::cross(planar_hitpt_vector, v));
				auto beta = glm::dot(w, glm::cross(u, planar_hitpt_vector));

				if (!is_interior(alpha, beta, rec))
					return;

				// Ray hits the 2D shape; set the rest of the hit record and return true.
				rec.at = t;
				rec.point = intersection;
				rec.setNormal(ray, normal);
				rec.mat = mat;
				rec.hit = true;
			}
			
			virtual bool is_interior(double a, double b, MreHitRec& rec) const
			{
				// Given the hit point in plane coordinates, return false if it is outside the
				// primitive, otherwise set the hit record UV coordinates and return true.

				if ((a < 0) || (1 < a) || (b < 0) || (1 < b))
					return false;

				rec.u = a;
				rec.v = b;
				return true;
			}
			
			void add(std::shared_ptr<MreObject> obj) override{}
		
		private:
			glm::dvec3 Q;
			glm::dvec3 u, v, w;
			glm::dvec3 normal;
			double D;
			bool seeBack;
			bool seeFront;
			std::shared_ptr<MreMaterial> mat;
	};
}
