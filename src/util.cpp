#include "util.hpp"
#include <string_view>

namespace forged_in_lost_lands_ecs
{
    std::size_t get_hashed_id(const char *name)
    {
        int hash = 5381;

        std::string_view view(name);
        // remove everything before the last space
        std::size_t pos = view.find_last_of(" ");
        view.remove_prefix(pos + 1);

        const char *new_name = view.data();

        for (int i = 0; i < view.size(); i++)
        {
            char c = new_name[i];
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
}