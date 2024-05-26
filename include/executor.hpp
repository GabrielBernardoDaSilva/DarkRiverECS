#pragma once

#include "forged_concepts.hpp"
#include "query.hpp"

namespace prometheus_ecs
{
	enum class ExecutorType
	{
		Startup,
		Update,
		End
	};

}