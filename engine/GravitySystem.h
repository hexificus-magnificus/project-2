#pragma once

namespace ECSEngine
{

template <typename... Components>
class GravitySystem : public System<Components...> {
public:
	const std::string & GetName() const override
	{ static std::string name("GravitySystem"); return name; }
	
	tSystemReturn Run(EngineView<Components...> *engine) override
	{
		auto& eManager = engine->GetEntityManager();
		for (auto e = eManager.begin(); e != eManager.end(); e++) {
			if (!eManager.template HasComponent<GravityComponent>(e->GetID())) {
				continue;
			}
			GravityComponent& grav = eManager.template GetComponent<GravityComponent>(e->GetID());
			// if something has a GravityComponent, it should also have a MovementComponent
			MovementComponent& vel = eManager.template GetComponent<MovementComponent>(e->GetID());

			if (grav.disabledTimer > 0) {
				grav.disabledTimer--;
			} else {
				float mul = vel.velocity.y > 0 ? 2.f : 1.f;
				vel.velocity = vel.velocity + mul * grav.acceleration;
			}
		}
		return kContinue;
	}
	
};


}



