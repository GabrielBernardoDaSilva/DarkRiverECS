# ForgedInLostLandsEcs
![alt text](https://github.com/GabrielBernardoDaSilva/forged-in-lost-lands-ecs/blob/master/forged-in-lost-lands.png)

ForgedInLostLandsEcs is an ECS (Entity-Component-System) library developed in C++23. It provides a powerful framework for building scalable and efficient game engines and simulations.


## Features

- **Entity Creation**: Easily create entities and attach components to them.
- **Event Handling**: Implement event-driven architecture with custom events and event handlers.
- **Schedulers**: Schedule tasks to be executed at specific intervals or delays.
- **Queries**: Efficiently query entities based on their components.
- **Systems**: Implement game logic and behaviors through systems that process entities based on queries.
- **Plugins**: Extend the functionality of the ECS with custom plugins.

## Requirements
- C++23 Compiler(clang)


## Example Usage

```cpp
#include <iostream>
#include <forged_in_lost_lands_ecs.hpp>

using namespace forged_in_lost_lands_ecs;

// Define components
struct Velocity : public Component {
    float x, y;
};

struct Health : public Component {
    int health;
};

struct Position : public Component {
    float x, y;
};

// Define events
struct Collision : public Event {
    bool collided;
};

// Define system functions
void check_collision(Query<Position&> query, EventManager& event_manager, Query<Velocity&> query1, EntityManager& entity_manager) {
    // Add entities
    entity_manager.add_entity(Position{.x = 100.0f, .y = 100.0f}, Velocity{.x = 100.0f, .y = 100.0f});
    
    // Subscribe to events
    event_manager.subscribe<Collision>([](Collision collision) {
        std::cout << "Collision: " << collision.collided << std::endl;
    });
    
    // Process entities
    auto a = query.all();
    auto b = query1.all();
    for (auto& [pos] : a) {
        std::cout << "Position: x: " << pos.x << " y: " << pos.y << std::endl;
        pos.x += 1.0f;
    }

    for (auto& [vel] : b) {
        std::cout << "Velocity: x: " << vel.x << " y: " << vel.y << std::endl;
        vel.x += 1.0f;
    }
}

void read_position(Query<const Position&> query) {
    for (auto& [pos] : query.all()) {
        std::cout << "Position: x: " << pos.x << " y: " << pos.y << std::endl;
    }
}

// Define a plugin
class PluginTest : public Plugin {
    virtual void build(LostLands& lost_lands) override {
        std::cout << "PluginTest on_start" << std::endl;
    }
};

int main() {
    LostLands lands;
    
    // Add entities
    Position pos = {.x = 10.0f, .y = 20.0f};
    Velocity vel = {.x = 5.0f, .y = 5.0f};
    Entity e = lands.add_entity(pos, vel);
    
    // Add components to entities
    lands.add_component_to_entity(e, Health{.health = 100});
    
    // Add system functions
    lands.add_executor(check_collision);
    lands.add_executor(read_position);
    
    // Run the ECS
    lands.run();
    
    // Emit an event
    lands.emit(Collision{.collided = true});
    
    // Add a task
    lands.add_task([](int i) {
        std::cout << "Task executed with argument: " << i << std::endl;
    }, 10);
    
    // Add a plugin
    lands.add_plugin<PluginTest>();
    
    // Build plugins
    lands.build_plugins();
    
    // Main loop
    while (true) {
        lands.run_tasks(0.16f);
    }
    
    return 0;
}
```


This README provides an overview of the library's features, example usage, requirements, installation instructions, and licensing information. Adjustments can be made as needed based on the specific details of the library.


## License
Forged in Lost Lands is licensed under the MIT License. See LICENSE for details.