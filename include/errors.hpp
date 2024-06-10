#pragma once

namespace winter_rain_ecs
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

	struct Success
	{
	};
}