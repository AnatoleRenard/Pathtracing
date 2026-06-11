#pragma once
#include "mre_model.hpp"
#include <memory>


namespace mre
{
	class MreWorld : public MreObject
	{
		public:
			MreWorld();
			~MreWorld();
			
			void add(std::shared_ptr<MreObject> obj) override
			{
				worldList.push_back(obj);
			}
			
			void hit(const MreRay& ray, MreHitRec& rec) override
			{
				for(const auto& obj : worldList)
				{
					obj->hit(ray, rec);
				}
			}
			
		private:
			std::vector<std::shared_ptr<MreObject>> worldList;
	};
}
