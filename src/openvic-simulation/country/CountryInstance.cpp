#include "CountryInstance.hpp"

#include "openvic-simulation/country/CountryDefinition.hpp"
#include "openvic-simulation/history/CountryHistory.hpp"
#include "openvic-simulation/map/Crime.hpp"
#include "openvic-simulation/map/MapInstance.hpp"
#include "openvic-simulation/misc/Define.hpp"
#include "openvic-simulation/politics/Ideology.hpp"
#include "openvic-simulation/research/Invention.hpp"
#include "openvic-simulation/research/Technology.hpp"

using namespace OpenVic;

using enum CountryInstance::country_status_t;

static constexpr colour_t ERROR_COLOUR = colour_t::from_integer(0xFF0000);

CountryInstance::CountryInstance(
	CountryDefinition const* new_country_definition,
	decltype(unlocked_building_types)::keys_t const& building_type_keys,
	decltype(unlocked_technologies)::keys_t const& technology_keys,
	decltype(unlocked_inventions)::keys_t const& invention_keys,
	decltype(upper_house)::keys_t const& ideology_keys,
	decltype(government_flag_overrides)::keys_t const& government_type_keys,
	decltype(unlocked_crimes)::keys_t const& crime_keys,
	decltype(pop_type_distribution)::keys_t const& pop_type_keys,
	decltype(unlocked_regiment_types)::keys_t const& unlocked_regiment_types_keys,
	decltype(unlocked_ship_types)::keys_t const& unlocked_ship_types_keys
) : /* Main attributes */
	country_definition { new_country_definition },
	colour { ERROR_COLOUR },
	capital { nullptr },
	country_flags {},
	releasable_vassal { true },
	country_status { COUNTRY_STATUS_UNCIVILISED },
	lose_great_power_date {},
	total_score { 0 },
	total_rank { 0 },
	owned_provinces {},
	controlled_provinces {},
	core_provinces {},
	states {},

	/* Production */
	industrial_power { 0 },
	industrial_power_from_states {},
	industrial_power_from_investments {},
	industrial_rank { 0 },
	foreign_investments {},
	unlocked_building_types { &building_type_keys },

	/* Budget */
	cash_stockpile { 0 },

	/* Technology */
	unlocked_technologies { &technology_keys },
	unlocked_inventions { &invention_keys },
	current_research { nullptr },
	invested_research_points { 0 },
	expected_completion_date {},
	research_point_stockpile { 0 },
	daily_research_points { 0 },
	national_literacy { 0 },
	tech_school { nullptr },

	/* Politics */
	national_value { nullptr },
	government_type { nullptr },
	last_election {},
	ruling_party { nullptr },
	upper_house { &ideology_keys },
	reforms {},
	government_flag_overrides { &government_type_keys },
	flag_government_type { nullptr },
	suppression_points { 0 },
	infamy { 0 },
	plurality { 0 },
	revanchism { 0 },
	unlocked_crimes { &crime_keys },

	/* Population */
	primary_culture { nullptr },
	accepted_cultures {},
	religion { nullptr },
	total_population { 0 },
	national_consciousness { 0 },
	national_militancy { 0 },
	pop_type_distribution { &pop_type_keys },
	national_focus_capacity { 0 },

	/* Trade */

	/* Diplomacy */
	prestige { 0 },
	prestige_rank { 0 },
	diplomatic_points { 0 },

	/* Military */
	military_power { 0 },
	military_power_from_land { 0 },
	military_power_from_sea { 0 },
	military_power_from_leaders { 0 },
	military_rank { 0 },
	generals {},
	admirals {},
	armies {},
	navies {},
	regiment_count { 0 },
	max_supported_regiment_count { 0 },
	mobilisation_potential_regiment_count { 0 },
	mobilisation_max_regiment_count { 0 },
	mobilisation_impact { 0 },
	supply_consumption { 1 },
	ship_count { 0 },
	total_consumed_ship_supply { 0 },
	max_ship_supply { 0 },
	leadership_points { 0 },
	war_exhaustion { 0 },
	mobilised { false },
	disarmed { false },
	unlocked_regiment_types { &unlocked_regiment_types_keys },
	allowed_regiment_cultures { RegimentType::allowed_cultures_t::NO_CULTURES },
	unlocked_ship_types { &unlocked_ship_types_keys },
	gas_attack_unlock_level { 0 },
	gas_defence_unlock_level { 0 },
	unit_variant_unlock_levels {} {

	for (BuildingType const& building_type : *unlocked_building_types.get_keys()) {
		if (building_type.is_default_enabled()) {
			unlock_building_type(building_type);
		}
	}

	for (Crime const& crime : *unlocked_crimes.get_keys()) {
		if (crime.is_default_active()) {
			unlock_crime(crime);
		}
	}

	for (RegimentType const& regiment_type : *unlocked_regiment_types.get_keys()) {
		if (regiment_type.is_active()) {
			unlock_unit_type(regiment_type);
		}
	}

	for (ShipType const& ship_type : *unlocked_ship_types.get_keys()) {
		if (ship_type.is_active()) {
			unlock_unit_type(ship_type);
		}
	}
}

std::string_view CountryInstance::get_identifier() const {
	return country_definition->get_identifier();
}

bool CountryInstance::exists() const {
	return !owned_provinces.empty();
}

bool CountryInstance::is_civilised() const {
	return country_status <= COUNTRY_STATUS_CIVILISED;
}

bool CountryInstance::can_colonise() const {
	return country_status <= COUNTRY_STATUS_SECONDARY_POWER;
}

bool CountryInstance::is_great_power() const {
	return country_status == COUNTRY_STATUS_GREAT_POWER;
}

bool CountryInstance::is_secondary_power() const {
	return country_status == COUNTRY_STATUS_SECONDARY_POWER;
}

bool CountryInstance::set_country_flag(std::string_view flag, bool warn) {
	if (flag.empty()) {
		Logger::error("Attempted to set empty country flag for country ", get_identifier());
		return false;
	}
	if (!country_flags.emplace(flag).second && warn) {
		Logger::warning(
			"Attempted to set country flag \"", flag, "\" for country ", get_identifier(), ": already set!"
		);
	}
	return true;
}

bool CountryInstance::clear_country_flag(std::string_view flag, bool warn) {
	if (flag.empty()) {
		Logger::error("Attempted to clear empty country flag from country ", get_identifier());
		return false;
	}
	if (country_flags.erase(flag) == 0 && warn) {
		Logger::warning(
			"Attempted to clear country flag \"", flag, "\" from country ", get_identifier(), ": not set!"
		);
	}
	return true;
}

#define ADD_AND_REMOVE(item) \
	bool CountryInstance::add_##item(std::remove_pointer_t<decltype(item##s)::value_type>& new_item) { \
		if (!item##s.emplace(&new_item).second) { \
			Logger::error( \
				"Attempted to add " #item " \"", new_item.get_identifier(), "\" to country ", get_identifier(), \
				": already present!" \
			); \
			return false; \
		} \
		return true; \
	} \
	bool CountryInstance::remove_##item(std::remove_pointer_t<decltype(item##s)::value_type>& item_to_remove) { \
		if (item##s.erase(&item_to_remove) == 0) { \
			Logger::error( \
				"Attempted to remove " #item " \"", item_to_remove.get_identifier(), "\" from country ", get_identifier(), \
				": not present!" \
			); \
			return false; \
		} \
		return true; \
	}

ADD_AND_REMOVE(owned_province)
ADD_AND_REMOVE(controlled_province)
ADD_AND_REMOVE(core_province)
ADD_AND_REMOVE(state)
ADD_AND_REMOVE(accepted_culture)

#undef ADD_AND_REMOVE

bool CountryInstance::set_upper_house(Ideology const* ideology, fixed_point_t popularity) {
	if (ideology != nullptr) {
		upper_house[*ideology] = popularity;
		return true;
	} else {
		Logger::error("Trying to set null ideology in upper house of ", get_identifier());
		return false;
	}
}

bool CountryInstance::add_reform(Reform const* new_reform) {
	if (std::find(reforms.begin(), reforms.end(), new_reform) != reforms.end()) {
		Logger::warning(
			"Attempted to add reform \"", new_reform, "\" to country ", get_identifier(), ": already present!"
		);
		return false;
	}
	reforms.push_back(new_reform);
	return true;
}

bool CountryInstance::remove_reform(Reform const* reform_to_remove) {
	auto existing_entry = std::find(reforms.begin(), reforms.end(), reform_to_remove);
	if (existing_entry == reforms.end()) {
		Logger::warning(
			"Attempted to remove reform \"", reform_to_remove, "\" from country ", get_identifier(), ": not present!"
		);
		return false;
	}
	reforms.erase(existing_entry);
	return true;
}

template<UnitType::branch_t Branch>
bool CountryInstance::add_unit_instance_group(UnitInstanceGroup<Branch>& group) {
	if (get_unit_instance_groups<Branch>().emplace(static_cast<UnitInstanceGroupBranched<Branch>*>(&group)).second) {
		return true;
	} else {
		Logger::error(
			"Trying to add already-existing ", Branch == UnitType::branch_t::LAND ? "army" : "navy", " ",
			group.get_name(), " to country ", get_identifier()
		);
		return false;
	}
}

template<UnitType::branch_t Branch>
bool CountryInstance::remove_unit_instance_group(UnitInstanceGroup<Branch>& group) {
	if (get_unit_instance_groups<Branch>().erase(static_cast<UnitInstanceGroupBranched<Branch>*>(&group)) > 0) {
		return true;
	} else {
		Logger::error(
			"Trying to remove non-existent ", Branch == UnitType::branch_t::LAND ? "army" : "navy", " ",
			group.get_name(), " from country ", get_identifier()
		);
		return false;
	}
}

template bool CountryInstance::add_unit_instance_group(UnitInstanceGroup<UnitType::branch_t::LAND>&);
template bool CountryInstance::add_unit_instance_group(UnitInstanceGroup<UnitType::branch_t::NAVAL>&);
template bool CountryInstance::remove_unit_instance_group(UnitInstanceGroup<UnitType::branch_t::LAND>&);
template bool CountryInstance::remove_unit_instance_group(UnitInstanceGroup<UnitType::branch_t::NAVAL>&);

template<UnitType::branch_t Branch>
void CountryInstance::add_leader(LeaderBranched<Branch>&& leader) {
	get_leaders<Branch>().emplace(std::move(leader));
}

template<UnitType::branch_t Branch>
bool CountryInstance::remove_leader(LeaderBranched<Branch> const* leader) {
	plf::colony<LeaderBranched<Branch>>& leaders = get_leaders<Branch>();
	const auto it = leaders.get_iterator(leader);
	if (it != leaders.end()) {
		leaders.erase(it);
		return true;
	}

	Logger::error(
		"Trying to remove non-existent ", Branch == UnitType::branch_t::LAND ? "general" : "admiral", " ",
		leader != nullptr ? leader->get_name() : "NULL", " from country ", get_identifier()
	);
	return false;
}

template void CountryInstance::add_leader(LeaderBranched<UnitType::branch_t::LAND>&&);
template void CountryInstance::add_leader(LeaderBranched<UnitType::branch_t::NAVAL>&&);
template bool CountryInstance::remove_leader(LeaderBranched<UnitType::branch_t::LAND> const*);
template bool CountryInstance::remove_leader(LeaderBranched<UnitType::branch_t::NAVAL> const*);

template<UnitType::branch_t Branch>
bool CountryInstance::modify_unit_type_unlock(UnitTypeBranched<Branch> const& unit_type, unlock_level_t unlock_level_change) {
	IndexedMap<UnitTypeBranched<Branch>, unlock_level_t>& unlocked_unit_types = get_unlocked_unit_types<Branch>();

	typename IndexedMap<UnitTypeBranched<Branch>, unlock_level_t>::value_ref_t unlock_level = unlocked_unit_types[unit_type];

	// This catches subtracting below 0 or adding above the int types maximum value
	if (unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for unit type ", unit_type.get_identifier(), " in country ",
			get_identifier(), " to invalid value: current level = ", static_cast<int64_t>(unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(unlock_level + unlock_level_change)
		);
		return false;
	}

	unlock_level += unlock_level_change;

	return true;
}

template bool CountryInstance::modify_unit_type_unlock(UnitTypeBranched<UnitType::branch_t::LAND> const&, unlock_level_t);
template bool CountryInstance::modify_unit_type_unlock(UnitTypeBranched<UnitType::branch_t::NAVAL> const&, unlock_level_t);

bool CountryInstance::modify_unit_type_unlock(UnitType const& unit_type, unlock_level_t unlock_level_change) {
	using enum UnitType::branch_t;

	switch (unit_type.get_branch()) {
	case LAND:
		return modify_unit_type_unlock(static_cast<UnitTypeBranched<LAND> const&>(unit_type), unlock_level_change);
	case NAVAL:
		return modify_unit_type_unlock(static_cast<UnitTypeBranched<NAVAL> const&>(unit_type), unlock_level_change);
	default:
		Logger::error(
			"Attempted to change unlock level for unit type \"", unit_type.get_identifier(), "\" with invalid branch ",
			static_cast<uint32_t>(unit_type.get_branch()), " is unlocked for country ", get_identifier()
		);
		return false;
	}
}

bool CountryInstance::unlock_unit_type(UnitType const& unit_type) {
	return modify_unit_type_unlock(unit_type, 1);
}

bool CountryInstance::is_unit_type_unlocked(UnitType const& unit_type) const {
	using enum UnitType::branch_t;

	switch (unit_type.get_branch()) {
	case LAND:
		return unlocked_regiment_types[static_cast<UnitTypeBranched<LAND> const&>(unit_type)] > 0;
	case NAVAL:
		return unlocked_ship_types[static_cast<UnitTypeBranched<NAVAL> const&>(unit_type)] > 0;
	default:
		Logger::error(
			"Attempted to check if unit type \"", unit_type.get_identifier(), "\" with invalid branch ",
			static_cast<uint32_t>(unit_type.get_branch()), " is unlocked for country ", get_identifier()
		);
		return false;
	}
}

bool CountryInstance::modify_building_type_unlock(BuildingType const& building_type, unlock_level_t unlock_level_change) {
	decltype(unlocked_building_types)::value_ref_t unlock_level = unlocked_building_types[building_type];

	// This catches subtracting below 0 or adding above the int types maximum value
	if (unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for building type ", building_type.get_identifier(), " in country ",
			get_identifier(), " to invalid value: current level = ", static_cast<int64_t>(unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(unlock_level + unlock_level_change)
		);
		return false;
	}

	unlock_level += unlock_level_change;

	return true;
}

bool CountryInstance::unlock_building_type(BuildingType const& building_type) {
	return modify_building_type_unlock(building_type, 1);
}

bool CountryInstance::is_building_type_unlocked(BuildingType const& building_type) const {
	return unlocked_building_types[building_type] > 0;
}

bool CountryInstance::modify_crime_unlock(Crime const& crime, unlock_level_t unlock_level_change) {
	decltype(unlocked_crimes)::value_ref_t unlock_level = unlocked_crimes[crime];

	// This catches subtracting below 0 or adding above the int types maximum value
	if (unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for crime ", crime.get_identifier(), " in country ",
			get_identifier(), " to invalid value: current level = ", static_cast<int64_t>(unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(unlock_level + unlock_level_change)
		);
		return false;
	}

	unlock_level += unlock_level_change;

	return true;
}

bool CountryInstance::unlock_crime(Crime const& crime) {
	return modify_crime_unlock(crime, 1);
}

bool CountryInstance::is_crime_unlocked(Crime const& crime) const {
	return unlocked_crimes[crime] > 0;
}

bool CountryInstance::modify_gas_attack_unlock(unlock_level_t unlock_level_change) {
	// This catches subtracting below 0 or adding above the int types maximum value
	if (gas_attack_unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for gas attack in country ", get_identifier(),
			" to invalid value: current level = ", static_cast<int64_t>(gas_attack_unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(gas_attack_unlock_level + unlock_level_change)
		);
		return false;
	}

	gas_attack_unlock_level += unlock_level_change;

	return true;
}

bool CountryInstance::unlock_gas_attack() {
	return modify_gas_attack_unlock(1);
}

bool CountryInstance::is_gas_attack_unlocked() const {
	return gas_attack_unlock_level > 0;
}

bool CountryInstance::modify_gas_defence_unlock(unlock_level_t unlock_level_change) {
	// This catches subtracting below 0 or adding above the int types maximum value
	if (gas_defence_unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for gas defence in country ", get_identifier(),
			" to invalid value: current level = ", static_cast<int64_t>(gas_defence_unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(gas_defence_unlock_level + unlock_level_change)
		);
		return false;
	}

	gas_defence_unlock_level += unlock_level_change;

	return true;
}

bool CountryInstance::unlock_gas_defence() {
	return modify_gas_defence_unlock(1);
}

bool CountryInstance::is_gas_defence_unlocked() const {
	return gas_defence_unlock_level > 0;
}

bool CountryInstance::modify_unit_variant_unlock(unit_variant_t unit_variant, unlock_level_t unlock_level_change) {
	if (unit_variant < 1) {
		Logger::error("Trying to modify unlock level for default unit variant 0");
		return false;
	}

	if (unit_variant_unlock_levels.size() < unit_variant) {
		unit_variant_unlock_levels.resize(unit_variant);
	}

	unlock_level_t& unlock_level = unit_variant_unlock_levels[unit_variant - 1];

	bool ret = true;

	// This catches subtracting below 0 or adding above the int types maximum value
	if (unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for unit variant ", static_cast<uint64_t>(unit_variant), " in country ",
			get_identifier(), " to invalid value: current level = ", static_cast<int64_t>(unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(unlock_level + unlock_level_change)
		);
		ret = false;
	} else {
		unlock_level += unlock_level_change;
	}

	while (!unit_variant_unlock_levels.empty() && unit_variant_unlock_levels.back() < 1) {
		unit_variant_unlock_levels.pop_back();
	}

	return ret;
}

bool CountryInstance::unlock_unit_variant(unit_variant_t unit_variant) {
	return modify_unit_variant_unlock(unit_variant, 1);
}

CountryInstance::unit_variant_t CountryInstance::get_max_unlocked_unit_variant() const {
	return unit_variant_unlock_levels.size();
}

bool CountryInstance::modify_technology_unlock(Technology const& technology, unlock_level_t unlock_level_change) {
	decltype(unlocked_technologies)::value_ref_t unlock_level = unlocked_technologies[technology];

	// This catches subtracting below 0 or adding above the int types maximum value
	if (unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for technology ", technology.get_identifier(), " in country ",
			get_identifier(), " to invalid value: current level = ", static_cast<int64_t>(unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(unlock_level + unlock_level_change)
		);
		return false;
	}

	unlock_level += unlock_level_change;

	bool ret = true;

	// TODO - bool unciv_military ?

	if (technology.get_unit_variant().has_value()) {
		ret &= modify_unit_variant_unlock(*technology.get_unit_variant(), unlock_level_change);
	}
	for (UnitType const* unit : technology.get_activated_units()) {
		ret &= modify_unit_type_unlock(*unit, unlock_level_change);
	}
	for (BuildingType const* building : technology.get_activated_buildings()) {
		ret &= modify_building_type_unlock(*building, unlock_level_change);
	}

	return ret;
}

bool CountryInstance::set_technology_unlock_level(Technology const& technology, unlock_level_t unlock_level) {
	const unlock_level_t unlock_level_change = unlock_level - unlocked_technologies[technology];
	return unlock_level_change != 0 ? modify_technology_unlock(technology, unlock_level_change) : true;
}

bool CountryInstance::unlock_technology(Technology const& technology) {
	return modify_technology_unlock(technology, 1);
}

bool CountryInstance::is_technology_unlocked(Technology const& technology) const {
	return unlocked_technologies[technology] > 0;
}

bool CountryInstance::modify_invention_unlock(Invention const& invention, unlock_level_t unlock_level_change) {
	decltype(unlocked_inventions)::value_ref_t unlock_level = unlocked_inventions[invention];

	// This catches subtracting below 0 or adding above the int types maximum value
	if (unlock_level + unlock_level_change < 0) {
		Logger::error(
			"Attempted to change unlock level for invention ", invention.get_identifier(), " in country ",
			get_identifier(), " to invalid value: current level = ", static_cast<int64_t>(unlock_level), ", change = ",
			static_cast<int64_t>(unlock_level_change), ", invalid new value = ",
			static_cast<int64_t>(unlock_level + unlock_level_change)
		);
		return false;
	}

	unlock_level += unlock_level_change;

	bool ret = true;

	// TODO - handle invention.is_news()

	for (UnitType const* unit : invention.get_activated_units()) {
		ret &= modify_unit_type_unlock(*unit, unlock_level_change);
	}
	for (BuildingType const* building : invention.get_activated_buildings()) {
		ret &= modify_building_type_unlock(*building, unlock_level_change);
	}
	for (Crime const* crime : invention.get_enabled_crimes()) {
		ret &= modify_crime_unlock(*crime, unlock_level_change);
	}
	if (invention.will_unlock_gas_attack()) {
		ret &= modify_gas_attack_unlock(unlock_level_change);
	}
	if (invention.will_unlock_gas_defence()) {
		ret &= modify_gas_defence_unlock(unlock_level_change);
	}

	return ret;
}

bool CountryInstance::set_invention_unlock_level(Invention const& invention, unlock_level_t unlock_level) {
	const unlock_level_t unlock_level_change = unlock_level - unlocked_inventions[invention];
	return unlock_level_change != 0 ? modify_invention_unlock(invention, unlock_level_change) : true;
}

bool CountryInstance::unlock_invention(Invention const& invention) {
	return modify_invention_unlock(invention, 1);
}

bool CountryInstance::is_invention_unlocked(Invention const& invention) const {
	return unlocked_inventions[invention] > 0;
}

bool CountryInstance::is_primary_culture(Culture const& culture) const {
	return &culture == primary_culture;
}

bool CountryInstance::is_accepted_culture(Culture const& culture) const {
	return accepted_cultures.contains(&culture);
}

bool CountryInstance::is_primary_or_accepted_culture(Culture const& culture) const {
	return is_primary_culture(culture) || is_accepted_culture(culture);
}

void CountryInstance::apply_foreign_investments(
	fixed_point_map_t<CountryDefinition const*> const& investments, CountryInstanceManager const& country_instance_manager
) {
	for (auto const& [country, money_invested] : investments) {
		foreign_investments[&country_instance_manager.get_country_instance_from_definition(*country)] = money_invested;
	}
}

bool CountryInstance::apply_history_to_country(
	CountryHistoryEntry const& entry, MapInstance& map_instance, CountryInstanceManager const& country_instance_manager
) {
	constexpr auto set_optional = []<typename T>(T& target, std::optional<T> const& source) {
		if (source) {
			target = *source;
		}
	};

	bool ret = true;

	set_optional(primary_culture, entry.get_primary_culture());
	for (Culture const* culture : entry.get_accepted_cultures()) {
		ret &= add_accepted_culture(*culture);
	}
	set_optional(religion, entry.get_religion());
	set_optional(ruling_party, entry.get_ruling_party());
	set_optional(last_election, entry.get_last_election());
	ret &= upper_house.copy(entry.get_upper_house());
	if (entry.get_capital()) {
		capital = &map_instance.get_province_instance_from_definition(**entry.get_capital());
	}
	set_optional(government_type, entry.get_government_type());
	set_optional(plurality, entry.get_plurality());
	set_optional(national_value, entry.get_national_value());
	if (entry.is_civilised()) {
		country_status = *entry.is_civilised() ? COUNTRY_STATUS_CIVILISED : COUNTRY_STATUS_UNCIVILISED;
	}
	set_optional(prestige, entry.get_prestige());
	for (Reform const* reform : entry.get_reforms()) {
		ret &= add_reform(reform);
	}
	set_optional(tech_school, entry.get_tech_school());
	constexpr auto set_bool_map_to_indexed_map =
		[]<typename T>(IndexedMap<T, bool>& target, ordered_map<T const*, bool> source) {
			for (auto const& [key, value] : source) {
				target[*key] = value;
			}
		};
	for (auto const& [technology, level] : entry.get_technologies()) {
		ret &= set_technology_unlock_level(*technology, level);
	}
	for (auto const& [invention, activated] : entry.get_inventions()) {
		ret &= set_invention_unlock_level(*invention, activated ? 1 : 0);
	}
	apply_foreign_investments(entry.get_foreign_investment(), country_instance_manager);

	// These need to be applied to pops
	// entry.get_consciousness();
	// entry.get_nonstate_consciousness();
	// entry.get_literacy();
	// entry.get_nonstate_culture_literacy();

	set_optional(releasable_vassal, entry.is_releasable_vassal());
	// entry.get_colonial_points();
	for (std::string const& flag : entry.get_country_flags()) {
		ret &= set_country_flag(flag, true);
	}
	for (std::string const& flag : entry.get_global_flags()) {
		// TODO - set global flag
	}
	government_flag_overrides.write_non_empty_values(entry.get_government_flag_overrides());
	for (Decision const* decision : entry.get_decisions()) {
		// TODO - take decision
	}

	return ret;
}

void CountryInstance::_update_production(DefineManager const& define_manager) {
	// Calculate industrial power from states and foreign investments
	industrial_power = 0;
	industrial_power_from_states.clear();
	industrial_power_from_investments.clear();

	for (State const* state : states) {
		const fixed_point_t state_industrial_power = state->get_industrial_power();
		if (state_industrial_power != 0) {
			industrial_power += state_industrial_power;
			industrial_power_from_states.emplace_back(state, state_industrial_power);
		}
	}

	for (auto const& [country, money_invested] : foreign_investments) {
		if (country->exists()) {
			const fixed_point_t investment_industrial_power =
				money_invested * define_manager.get_country_investment_industrial_score_factor() / 100;

			if (investment_industrial_power != 0) {
				industrial_power += investment_industrial_power;
				industrial_power_from_investments.emplace_back(country, investment_industrial_power);
			}
		}
	}

	std::sort(
		industrial_power_from_states.begin(), industrial_power_from_states.end(),
		[](auto const& a, auto const& b) -> bool { return a.second > b.second; }
	);
	std::sort(
		industrial_power_from_investments.begin(), industrial_power_from_investments.end(),
		[](auto const& a, auto const& b) -> bool { return a.second > b.second; }
	);
}

void CountryInstance::_update_budget() {

}

void CountryInstance::_update_technology() {

}

void CountryInstance::_update_politics() {

}

void CountryInstance::_update_population() {
	total_population = 0;
	national_literacy = 0;
	national_consciousness = 0;
	national_militancy = 0;
	pop_type_distribution.clear();

	for (State const* state : states) {
		total_population += state->get_total_population();

		// TODO - change casting if Pop::pop_size_t changes type
		const fixed_point_t state_population = fixed_point_t::parse(state->get_total_population());
		national_literacy += state->get_average_literacy() * state_population;
		national_consciousness += state->get_average_consciousness() * state_population;
		national_militancy += state->get_average_militancy() * state_population;

		pop_type_distribution += state->get_pop_type_distribution();
	}

	if (total_population > 0) {
		national_literacy /= total_population;
		national_consciousness /= total_population;
		national_militancy /= total_population;
	}

	// TODO - update national focus capacity
}

void CountryInstance::_update_trade() {
	// TODO - update total amount of each good exported and imported
}

void CountryInstance::_update_diplomacy() {
	// TODO - add prestige from modifiers
	// TODO - update diplomatic points and colonial power
}

void CountryInstance::_update_military(DefineManager const& define_manager, UnitTypeManager const& unit_type_manager) {
	regiment_count = 0;

	for (ArmyInstance const* army : armies) {
		regiment_count += army->get_unit_count();
	}

	ship_count = 0;
	total_consumed_ship_supply = 0;

	for (NavyInstance const* navy : navies) {
		ship_count += navy->get_unit_count();
		total_consumed_ship_supply += navy->get_total_consumed_supply();
	}

	// Calculate military power from land, sea, and leaders

	size_t deployed_non_mobilised_regiments = 0;
	for (ArmyInstance const* army : armies) {
		for (RegimentInstance const* regiment : army->get_units()) {
			if (!regiment->is_mobilised()) {
				deployed_non_mobilised_regiments++;
			}
		}
	}

	max_supported_regiment_count = 0;
	for (State const* state : states) {
		max_supported_regiment_count += state->get_max_supported_regiments();
	}

	// TODO - apply country/tech modifiers to supply consumption
	supply_consumption = 1;

	const size_t regular_army_size = std::min(4 * deployed_non_mobilised_regiments, max_supported_regiment_count);

	fixed_point_t sum_of_regiment_type_stats = 0;
	for (RegimentType const& regiment_type : unit_type_manager.get_regiment_types()) {
		// TODO - apply country/tech modifiers to regiment stats
		sum_of_regiment_type_stats += (
			regiment_type.get_attack() + regiment_type.get_defence() /*+ land_attack_modifier + land_defense_modifier*/
		) * regiment_type.get_discipline();
	}

	military_power_from_land = supply_consumption * fixed_point_t::parse(regular_army_size) * sum_of_regiment_type_stats
		/ fixed_point_t::parse(7 * (1 + unit_type_manager.get_regiment_type_count()));

	if (disarmed) {
		military_power_from_land *= define_manager.get_disarmed_penalty();
	}

	military_power_from_sea = 0;
	for (NavyInstance const* navy : navies) {
		for (ShipInstance const* ship : navy->get_units()) {
			ShipType const& ship_type = ship->get_unit_type();

			if (ship_type.is_capital()) {

				// TODO - include gun power and hull modifiers + naval attack and defense modifiers

				military_power_from_sea += (ship_type.get_gun_power() /*+ naval_attack_modifier*/)
					* (ship_type.get_hull() /* + naval_defense_modifier*/);
			}
		}
	}
	military_power_from_sea /= 250;

	military_power_from_leaders = fixed_point_t::parse(
		std::min(generals.size() + admirals.size(), deployed_non_mobilised_regiments)
	);

	military_power = military_power_from_land + military_power_from_sea + military_power_from_leaders;

	// Mobilisation calculations
	mobilisation_impact = 0; // TODO - apply ruling party's war policy

	mobilisation_max_regiment_count =
		((fixed_point_t::_1() + mobilisation_impact) * fixed_point_t::parse(regiment_count)).to_int64_t();

	mobilisation_potential_regiment_count = 0; // TODO - calculate max regiments from poor citizens
	if (mobilisation_potential_regiment_count > mobilisation_max_regiment_count) {
		mobilisation_potential_regiment_count = mobilisation_max_regiment_count;
	}

	// TODO - update max_ship_supply, leadership_points, war_exhaustion
}

void CountryInstance::update_gamestate(DefineManager const& define_manager, UnitTypeManager const& unit_type_manager) {
	// Order of updates might need to be changed/functions split up to account for dependencies
	_update_production(define_manager);
	_update_budget();
	_update_technology();
	_update_politics();
	_update_population();
	_update_trade();
	_update_diplomacy();
	_update_military(define_manager, unit_type_manager);

	total_score = prestige + industrial_power + military_power;

	if (country_definition != nullptr) {
		const CountryDefinition::government_colour_map_t::const_iterator it =
			country_definition->get_alternative_colours().find(government_type);

		if (it != country_definition->get_alternative_colours().end()) {
			colour = it->second;
		} else {
			colour = country_definition->get_colour();
		}
	} else {
		colour = ERROR_COLOUR;
	}

	if (government_type != nullptr) {
		flag_government_type = government_flag_overrides[*government_type];

		if (flag_government_type == nullptr) {
			flag_government_type = government_type;
		}
	} else {
		flag_government_type = nullptr;
	}
}

void CountryInstance::tick() {

}

void CountryInstanceManager::update_rankings(Date today, DefineManager const& define_manager) {
	total_ranking.clear();

	for (CountryInstance& country : country_instances.get_items()) {
		if (country.exists()) {
			total_ranking.push_back(&country);
		}
	}

	prestige_ranking = total_ranking;
	industrial_power_ranking = total_ranking;
	military_power_ranking = total_ranking;

	std::sort(
		total_ranking.begin(), total_ranking.end(),
		[](CountryInstance const* a, CountryInstance const* b) -> bool {
			const bool a_civilised = a->is_civilised();
			const bool b_civilised = b->is_civilised();
			return a_civilised != b_civilised ? a_civilised : a->get_total_score() > b->get_total_score();
		}
	);
	std::sort(
		prestige_ranking.begin(), prestige_ranking.end(),
		[](CountryInstance const* a, CountryInstance const* b) -> bool {
			return a->get_prestige() > b->get_prestige();
		}
	);
	std::sort(
		industrial_power_ranking.begin(), industrial_power_ranking.end(),
		[](CountryInstance const* a, CountryInstance const* b) -> bool {
			return a->get_industrial_power() > b->get_industrial_power();
		}
	);
	std::sort(
		military_power_ranking.begin(), military_power_ranking.end(),
		[](CountryInstance const* a, CountryInstance const* b) -> bool {
			return a->get_military_power() > b->get_military_power();
		}
	);

	for (size_t index = 0; index < total_ranking.size(); ++index) {
		const size_t rank = index + 1;
		total_ranking[index]->total_rank = rank;
		prestige_ranking[index]->prestige_rank = rank;
		industrial_power_ranking[index]->industrial_rank = rank;
		military_power_ranking[index]->military_rank = rank;
	}

	const size_t max_great_power_rank = define_manager.get_great_power_rank();
	const size_t max_secondary_power_rank = define_manager.get_secondary_power_rank();
	const Timespan lose_great_power_grace_days = define_manager.get_lose_great_power_grace_days();

	// Demote great powers who have been below the max great power rank for longer than the demotion grace period and
	// remove them from the list. We don't just demote them all and clear the list as when rebuilding we'd need to look
	// ahead for countries below the max great power rank but still within the demotion grace period.
	for (CountryInstance* great_power : great_powers) {
		if (great_power->get_total_rank() > max_great_power_rank && great_power->get_lose_great_power_date() < today) {
			great_power->country_status = COUNTRY_STATUS_CIVILISED;
		}
	}
	std::erase_if(great_powers, [](CountryInstance const* country) -> bool {
		return country->get_country_status() != COUNTRY_STATUS_GREAT_POWER;
	});

	// Demote all secondary powers and clear the list. We will rebuilt the whole list from scratch, so there's no need to
	// keep countries which are still above the max secondary power rank (they might become great powers instead anyway).
	for (CountryInstance* secondary_power : secondary_powers) {
		secondary_power->country_status = COUNTRY_STATUS_CIVILISED;
	}
	secondary_powers.clear();

	// Calculate the maximum number of countries eligible for great or secondary power status. This accounts for the
	// possibility of the max secondary power rank being higher than the max great power rank or both being zero, just
	// in case someone wants to experiment with only having secondary powers when some great power slots are filled by
	// countries in the demotion grace period, or having no great or secondary powers at all.
	const size_t max_power_index = std::clamp(max_secondary_power_rank, max_great_power_rank, total_ranking.size());

	for (size_t index = 0; index < max_power_index; index++) {
		CountryInstance* country = total_ranking[index];

		if (!country->is_civilised()) {
			// All further countries are civilised and so ineligible for great or secondary power status.
			break;
		}

		if (country->is_great_power()) {
			// The country already has great power status and is in the great powers list.
			continue;
		}

		if (great_powers.size() < max_great_power_rank && country->get_total_rank() <= max_great_power_rank) {
			// The country is eligible for great power status and there are still slots available,
			// so it is promoted and added to the list.
			country->country_status = COUNTRY_STATUS_GREAT_POWER;
			great_powers.push_back(country);
		} else if (country->get_total_rank() <= max_secondary_power_rank) {
			// The country is eligible for secondary power status and so is promoted and added to the list.
			country->country_status = COUNTRY_STATUS_SECONDARY_POWER;
			secondary_powers.push_back(country);
		}
	}

	// Sort the great powers list by total rank, as pre-existing great powers may have changed rank order and new great
	// powers will have beeen added to the end of the list regardless of rank.
	std::sort(great_powers.begin(), great_powers.end(), [](CountryInstance const* a, CountryInstance const* b) -> bool {
		return a->get_total_rank() < b->get_total_rank();
	});

	// Update the lose great power date for all great powers which are above the max great power rank.
	const Date new_lose_great_power_date = today + lose_great_power_grace_days;
	for (CountryInstance* great_power : great_powers) {
		if (great_power->get_total_rank() <= max_great_power_rank) {
			great_power->lose_great_power_date = new_lose_great_power_date;
		}
	}
}

CountryInstance& CountryInstanceManager::get_country_instance_from_definition(CountryDefinition const& country) {
	return country_instances.get_items()[country.get_index()];
}

CountryInstance const& CountryInstanceManager::get_country_instance_from_definition(CountryDefinition const& country) const {
	return country_instances.get_items()[country.get_index()];
}

bool CountryInstanceManager::generate_country_instances(
	CountryDefinitionManager const& country_definition_manager,
	decltype(CountryInstance::unlocked_building_types)::keys_t const& building_type_keys,
	decltype(CountryInstance::unlocked_technologies)::keys_t const& technology_keys,
	decltype(CountryInstance::unlocked_inventions)::keys_t const& invention_keys,
	decltype(CountryInstance::upper_house)::keys_t const& ideology_keys,
	decltype(CountryInstance::government_flag_overrides)::keys_t const& government_type_keys,
	decltype(CountryInstance::unlocked_crimes)::keys_t const& crime_keys,
	decltype(CountryInstance::pop_type_distribution)::keys_t const& pop_type_keys,
	decltype(CountryInstance::unlocked_regiment_types)::keys_t const& unlocked_regiment_types_keys,
	decltype(CountryInstance::unlocked_ship_types)::keys_t const& unlocked_ship_types_keys
) {
	reserve_more(country_instances, country_definition_manager.get_country_definition_count());

	bool ret = true;

	for (CountryDefinition const& country_definition : country_definition_manager.get_country_definitions()) {
		ret &= country_instances.add_item({
			&country_definition,
			building_type_keys,
			technology_keys,
			invention_keys,
			ideology_keys,
			government_type_keys,
			crime_keys,
			pop_type_keys,
			unlocked_regiment_types_keys,
			unlocked_ship_types_keys
		});
	}

	return ret;
}

bool CountryInstanceManager::apply_history_to_countries(
	CountryHistoryManager const& history_manager, Date date, UnitInstanceManager& unit_instance_manager,
	MapInstance& map_instance
) {
	bool ret = true;

	for (CountryInstance& country_instance : country_instances.get_items()) {
		if (!country_instance.get_country_definition()->is_dynamic_tag()) {
			CountryHistoryMap const* history_map =
				history_manager.get_country_history(country_instance.get_country_definition());

			if (history_map != nullptr) {
				CountryHistoryEntry const* oob_history_entry = nullptr;

				for (auto const& [entry_date, entry] : history_map->get_entries()) {
					if (entry_date <= date) {
						ret &= country_instance.apply_history_to_country(*entry, map_instance, *this);

						if (entry->get_inital_oob()) {
							oob_history_entry = entry.get();
						}
					} else {
						// All foreign investments are applied regardless of the bookmark's date
						country_instance.apply_foreign_investments(entry->get_foreign_investment(), *this);
					}
				}

				if (oob_history_entry != nullptr) {
					ret &= unit_instance_manager.generate_deployment(
						map_instance, country_instance, *oob_history_entry->get_inital_oob()
					);
				}
			} else {
				Logger::error("Country ", country_instance.get_identifier(), " has no history!");
				ret = false;
			}
		}
	}

	return ret;
}

void CountryInstanceManager::update_gamestate(
	Date today, DefineManager const& define_manager, UnitTypeManager const& unit_type_manager
) {
	for (CountryInstance& country : country_instances.get_items()) {
		country.update_gamestate(define_manager, unit_type_manager);
	}

	update_rankings(today, define_manager);
}

void CountryInstanceManager::tick() {
	for (CountryInstance& country : country_instances.get_items()) {
		country.tick();
	}
}
