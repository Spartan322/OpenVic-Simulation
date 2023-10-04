#pragma once

#include "openvic-simulation/types/IdentifierRegistry.hpp"
#include "openvic-simulation/politics/Ideology.hpp"

namespace OpenVic {
    struct GovernmentTypeManager;

    struct GovernmentType : HasIdentifier {
        friend struct GovernmentTypeManager;

    private:
        std::vector<Ideology const*> ideologies;
        const bool elections, appoint_ruling_party;
        const Timespan election_duration;
        const std::string flag_type_identifier;

        GovernmentType(std::string_view new_identifier, std::vector<Ideology const*> new_ideologies, bool new_elections, bool new_appoint_ruling_party, Timespan new_election_duration, std::string_view new_flag_type_identifier);

    public:
        GovernmentType(GovernmentType&&) = default;

        bool is_ideology_compatible(Ideology const* ideology) const;
        std::vector<Ideology const*> const& get_ideologies() const;
        bool holds_elections() const;
        bool can_appoint_ruling_party() const;
        Timespan get_election_duration() const;
        std::string_view get_flag_type() const;
    };

    struct GovernmentTypeManager {
    private:
        IdentifierRegistry<GovernmentType> government_types;

    public:
        GovernmentTypeManager();

        bool add_government_type(std::string_view identifier, std::vector<Ideology const*> ideologies, bool elections, bool appoint_ruling_party, Timespan election_duration, std::string_view flag_type);
        IDENTIFIER_REGISTRY_ACCESSORS(GovernmentType, government_type)

        bool load_government_types_file(IdeologyManager const& ideology_manager, ast::NodeCPtr root);
    };
} // namespace OpenVic 