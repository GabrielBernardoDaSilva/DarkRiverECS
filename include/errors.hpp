#pragma once

namespace forged_in_lost_lands_ecs
{
	enum class archetype_error
	{
		ENTITY_NOT_FOUND,
		COMPONENT_NOT_FOUND,
		COMPONENTS_TYPE_DOES_NOT_MATCH
	};

	enum class SchedulerError
	{
		TASK_NOT_FOUND
	};

	struct Success{};
}