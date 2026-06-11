#pragma once
#include "mre_object.hpp"
#include <cmath>

namespace mre
{
	class MreSphere : public MreObject
	{
		public:
			MreSphere(const glm::dvec3& center, const std::shared_ptr<MreMaterial> mat, double radius) : center(center), mat(mat), radius(radius){}
			
			void hit(const MreRay& ray, MreHitRec& rec) override
			{
				glm::dvec3 oc = ray.origin - center;
				double a = glm::length(ray.direction);
				double half_b = glm::dot(oc, ray.direction);
				double c = glm::dot(oc, oc) - radius*radius;

				auto discriminant = half_b*half_b - a*c;
				if (discriminant < epsilon) return;
				auto sqrtd = sqrt(discriminant);

				// Find the nearest root that lies in the acceptable range.
				auto root = (-half_b - sqrtd) / a;
				if (root <= rec.min || rec.at <= root) {
					root = (-half_b + sqrtd) / a;
					if (root <= rec.min || rec.at <= root)
						return;
				}

				rec.at = root;
				rec.point = ray.at(rec.at);
				rec.setNormal(ray, (rec.point - center) / radius);
				rec.mat = mat;
				rec.hit = true;
				sphereUV((rec.point - center) / radius, rec.u, rec.v);
			}
			
			void add(std::shared_ptr<MreObject> obj) override{}
		
		private:
			std::shared_ptr<MreMaterial> mat;
			double radius;
			glm::dvec3 center;
			
			static void sphereUV(const glm::dvec3& p, double& u, double& v)
			{
				// p: a given point on the sphere of radius one, centered at the origin.
				// u: returned value [0,1] of angle around the Y axis from X=-1.
				// v: returned value [0,1] of angle from Y=-1 to Y=+1.
				//     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
				//     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
				//     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

				auto theta = acos(-p.y);
				auto phi = atan2(-p.z, p.x) + pi;

				u = phi / (2*pi);
				v = theta / pi;
			}
	};
}
