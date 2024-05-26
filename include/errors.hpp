#pragma once

namespace forged_in_lost_lands_ecs
{
	enum class ArchetypeError
	{
		EntityNotFound,
		ComponentNotFound,
		ComponentTypeDoesNotMatch,
		EntityAlreadyHasComponent,
		EntityDoesNotHaveComponent,
	};

	enum class SchedulerError
	{
		TaskNotFound
	};

	struct Success{};
}