#pragma once
#include "Engine/Scene/Systems/System.h"
#include "Lib/Entities/Components/CastingSpell.h"
#include "Lib/Entities/ActionQueue.h"
#include "Lib/Data/SpellTypes.h"

namespace Anarchy
{

	class SpellManager : public System<SpellManager>
	{
	private:
		ActionQueue* m_Actions;
	
	public:
		SpellManager(ActionQueue* actions);

		void Update(EntityManager& entities, TimeDelta delta) override;

	private:
		template<typename T>
		T CreateCastData(const OutputMemoryStream& data)
		{
			T result;
			InputMemoryStream stream = InputMemoryStream::FromStream(data);
			Deserialize(stream, result);
			return result;
		}

		CastSpellAction CastSpell(entityid_t networkId, const EntityHandle& entity, const CCastingSpell& spell);

		CastSpellAction CastSpellInternal(entityid_t networkId, const EntityHandle& entity, const FireballCastData& data);
	};

}