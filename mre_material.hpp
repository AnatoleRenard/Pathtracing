#pragma once

#include "mre_texture.hpp"
#include "mre_ray.hpp"
#include "mre_hitrec.hpp"

namespace mre
{		
	class MreMaterial
	{
		public:
			virtual ~MreMaterial() = default;

			virtual bool scatter(const MreRay& ray, const MreHitRec& rec, glm::dvec3& attenuation, MreRay& scattered) const = 0;
			virtual void emitted(double u, double v, const glm::dvec3& p, glm::dvec3& light) const
			{
				light = glm::dvec3(0.0);
			};
	};
	
	class MreLambertian : public MreMaterial {
		public:
			MreLambertian(std::shared_ptr<MreTexture> a) : albedo(a) {}

			bool scatter(const MreRay& ray, const MreHitRec& rec, glm::dvec3& attenuation, MreRay& scattered) const override
			{
				auto scatter_direction = rec.normal + randomUnitVector();
				
				// Catch degenerate scatter direction
				if (nearZero(scatter_direction))
					scatter_direction = rec.normal;
				
				scattered = MreRay(rec.point, scatter_direction);
				attenuation = albedo->value(rec.u, rec.v, rec.point);
				return true;
			}
		
		
		private:
			std::shared_ptr<MreTexture> albedo;
	};
	
	class MreMetal : public MreMaterial {
		public:
			MreMetal(const std::shared_ptr<MreTexture> col, double f) : albedo(col), fuzz(f < 1.0 ? f : 1.0) {}

			bool scatter(const MreRay& ray, const MreHitRec& rec, glm::dvec3& attenuation, MreRay& scattered) const override
			{
				glm::dvec3 reflected = reflect(glm::normalize(ray.direction), rec.normal);
				scattered = MreRay(rec.point, reflected + fuzz*randomUnitVector());
				attenuation = albedo->value(rec.u, rec.v, rec.point);
				return (glm::dot(scattered.direction, rec.normal) > epsilon);
			}
			

		private:
			std::shared_ptr<MreTexture> albedo;
			double fuzz;
	};
	
	class MreDielectric : public MreMaterial
	{
		public:
			MreDielectric(const std::shared_ptr<MreTexture> tint, double indexRefraction) : albedo(tint), ir(indexRefraction){}
			
			bool scatter(const MreRay& ray, const MreHitRec& rec, glm::dvec3& attenuation, MreRay& scattered) const override
			{
				double refraction_ratio = rec.frontFace ? (1.0/ir) : ir;

				glm::dvec3 unit_direction = glm::normalize(ray.direction);
				double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0);
				double sin_theta = sqrt(1.0 - cos_theta*cos_theta);

				bool cannot_refract = refraction_ratio * sin_theta > 1.0;
				glm::dvec3 direction;

				if (cannot_refract || reflectance(cos_theta, refraction_ratio) > randomDouble())
					direction = reflect(unit_direction, rec.normal);
				else
					direction = refract(unit_direction, rec.normal, refraction_ratio);

				scattered = MreRay(rec.point, direction);
				attenuation = albedo->value(rec.u, rec.v, rec.point);
				return true;
			}
			
			
		private:
			double ir;
			std::shared_ptr<MreTexture> albedo;
			
			static double reflectance(double cosine, double ref_idx)
			{
				// Use Schlick's approximation for reflectance.
				auto r0 = (1-ref_idx) / (1+ref_idx);
				r0 = r0*r0;
				return r0 + (1-r0)*pow((1 - cosine),5);
			}
	};
	
	class MreLight : public MreMaterial
	{
		public:
			MreLight(const std::shared_ptr<MreTexture> tex) : col(tex){}
		
			bool scatter(const MreRay& ray, const MreHitRec& rec, glm::dvec3& attenuation, MreRay& out) const override
			{
				return false;
			}
			
			void emitted(double u, double v, const glm::dvec3& p, glm::dvec3& light) const override 
			{
				light = col->value(u, v, p);
			}
			
		private:
			std::shared_ptr<MreTexture> col;
	};
	
	class MreIsotropic : public MreMaterial
	{
		public:
			MreIsotropic(const std::shared_ptr<MreTexture>& tex) : albedo(tex){}
			
			bool scatter(const MreRay& ray, const MreHitRec& rec, glm::dvec3& attenuation, MreRay& out) const override
			{
				out = MreRay(rec.point, randomUnitVector());
				attenuation = albedo->value(rec.u, rec.v, rec.point);
				return true;
			}
			
		private:
			std::shared_ptr<MreTexture> albedo;
	};
}
