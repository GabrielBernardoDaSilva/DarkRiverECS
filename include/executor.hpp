#pragma once

#include "forged_concepts.hpp"
#include "query.hpp"

namespace forged_in_lost_lands_ecs
{
	enum class ExecutorType
	{
		Startup,
		Update,
		End
	};

}