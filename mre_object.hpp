#pragma once
#include <memory>
#include "mre_ray.hpp"
#include "mre_material.hpp"
#include "mre_hitrec.hpp"

namespace mre
{
	class MreObject
	{
		public:
			virtual ~MreObject() = default;
		
			virtual void hit(const MreRay& ray, MreHitRec& rec) = 0;
			virtual void add(std::shared_ptr<MreObject> obj) = 0;
	};
}
