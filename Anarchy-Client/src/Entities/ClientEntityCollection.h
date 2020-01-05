#pragma once
#include "Lib/Entities/EntityCollection.h"

namespace Anarchy
{

	class ClientEntityCollection : public EntityCollection
	{
	public:
		ClientEntityCollection(Scene& scene, Layer& layer);
	};

}