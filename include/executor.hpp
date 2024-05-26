#pragma once

#include "forged_concepts.hpp"
#include "query.hpp"

namespace winter_rain_ecs
{
	enum class ExecutorType
	{
		Startup,
		Update,
		End
	};

}