#include "CountryDefines.hpp"

using namespace OpenVic;
using namespace OpenVic::NodeTools;

CountryDefines::CountryDefines()
  : nationalism_duration {},
	rebels_hold_capital_success_duration {}, // NOT USED
	rebel_success_duration {},
	base_country_tax_efficiency {},
	base_country_admin_efficiency {},
	gold_to_cash_rate {},
	gold_to_worker_pay_rate {},
	great_power_rank {},
	lose_great_power_grace_days {},
	infamy_containment_limit {},
	max_bureaucracy_percentage {},
	bureaucracy_percentage_increment {},
	min_crimefight_percent {},
	max_crimefight_percent {},
	admin_efficiency_crimefight_percent {},
	conservative_increase_after_reform {},
	campaign_event_base_duration {},
	campaign_event_min_duration {}, // NOT USED
	campaign_event_state_duration_modifier {}, // NOT USED
	campaign_duration {},
	secondary_power_rank {},
	colony_to_state_prestige_gain {},
	colonial_liferating {},
	base_greatpower_daily_influence {},
	ai_support_reform {},
	base_monthly_diplopoints {},
	diplomat_travel_duration {},
	province_overseas_penalty {},
	noncore_tax_penalty {},
	base_tariff_efficiency {},
	colony_formed_prestige {},
	created_cb_valid_time {},
	loyalty_boost_on_party_win {},
	movement_radicalism_base {},
	movement_radicalism_passed_reform_effect {},
	movement_radicalism_nationalism_factor {},
	suppression_points_gain_base {},
	suppress_bureaucrat_factor {},
	wrong_reform_militancy_impact {},
	suppression_radicalisation_hit {},
	country_investment_industrial_score_factor {},
	unciv_tech_spread_max {},
	unciv_tech_spread_min {},
	min_delay_duration_between_reforms {},
	economic_reform_uh_factor {},
	military_reform_uh_factor {},
	wrong_reform_radical_impact {},
	tech_year_span {},
	tech_factor_vassal {},
	max_suppression {},
	prestige_hit_on_break_country {},
	min_mobilize_limit {},
	pop_growth_country_cache_days {},
	newspaper_printing_frequency {},
	newspaper_timeout_period {},
	newspaper_max_tension {},
	naval_base_supply_score_base {},
	naval_base_supply_score_empty {},
	naval_base_non_core_supply_score {},
	colonial_points_from_supply_factor {},
	colonial_points_for_non_core_base {},
	mobilization_speed_base {},
	mobilization_speed_rails_mult {},
	colonization_interest_lead {},
	colonization_influence_lead {},
	colonization_duration {},
	colonization_days_between_investment {},
	colonization_days_for_initial_investment {},
	colonization_protectorate_province_maintainance {},
	colonization_colony_province_maintainance {},
	colonization_colony_industry_maintainance {},
	colonization_colony_railway_maintainance {},
	colonization_interest_cost_initial {},
	colonization_interest_cost_neighbor_modifier {},
	colonization_interest_cost {},
	colonization_influence_cost {},
	colonization_extra_guard_cost {},
	colonization_release_dominion_cost {},
	colonization_create_state_cost {},
	colonization_create_protectorate_cost {},
	colonization_create_colony_cost {},
	colonization_colony_state_distance {},
	colonization_influence_temperature_per_day {},
	colonization_influence_temperature_per_level {},
	party_loyalty_hit_on_war_loss {},
	research_points_on_conquer_mult {},
	max_research_points {} {}

std::string_view CountryDefines::get_name() const {
	return "country";
}

node_callback_t CountryDefines::expect_defines() {
	return expect_dictionary_keys(
		"YEARS_OF_NATIONALISM", ONE_EXACTLY, expect_years(assign_variable_callback(nationalism_duration)),
		"MONTHS_UNTIL_BROKEN", ZERO_OR_ONE, // NOT USED
			expect_months(assign_variable_callback(rebels_hold_capital_success_duration)),
		"REBEL_ACCEPTANCE_MONTHS", ONE_EXACTLY, expect_months(assign_variable_callback(rebel_success_duration)),
		"BASE_COUNTRY_TAX_EFFICIENCY", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(base_country_tax_efficiency)),
		"BASE_COUNTRY_ADMIN_EFFICIENCY", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(base_country_admin_efficiency)),
		"GOLD_TO_CASH_RATE", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(gold_to_cash_rate)),
		"GOLD_TO_WORKER_PAY_RATE", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(gold_to_worker_pay_rate)),
		"GREAT_NATIONS_COUNT", ONE_EXACTLY, expect_uint(assign_variable_callback(great_power_rank)),
		"GREATNESS_DAYS", ONE_EXACTLY, expect_days(assign_variable_callback(lose_great_power_grace_days)),
		"BADBOY_LIMIT", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(infamy_containment_limit)),
		"MAX_BUREAUCRACY_PERCENTAGE", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(max_bureaucracy_percentage)),
		"BUREAUCRACY_PERCENTAGE_INCREMENT", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(bureaucracy_percentage_increment)),
		"MIN_CRIMEFIGHT_PERCENT", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(min_crimefight_percent)),
		"MAX_CRIMEFIGHT_PERCENT", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(max_crimefight_percent)),
		"ADMIN_EFFICIENCY_CRIMEFIGHT_PERCENT", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(admin_efficiency_crimefight_percent)),
		"CONSERVATIVE_INCREASE_AFTER_REFORM", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(conservative_increase_after_reform)),
		"CAMPAIGN_EVENT_BASE_TIME", ONE_EXACTLY, expect_days(assign_variable_callback(campaign_event_base_duration)),
		"CAMPAIGN_EVENT_MIN_TIME", ZERO_OR_ONE, expect_days(assign_variable_callback(campaign_event_min_duration)), // NOT USED
		"CAMPAIGN_EVENT_STATE_SCALE", ZERO_OR_ONE, // NOT USED
			expect_days(assign_variable_callback(campaign_event_state_duration_modifier)),
		"CAMPAIGN_DURATION", ONE_EXACTLY, expect_months(assign_variable_callback(campaign_duration)),
		"COLONIAL_RANK", ONE_EXACTLY, expect_uint(assign_variable_callback(secondary_power_rank)),
		"COLONY_TO_STATE_PRESTIGE_GAIN", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colony_to_state_prestige_gain)),
		"COLONIAL_LIFERATING", ONE_EXACTLY, expect_uint(assign_variable_callback(colonial_liferating)),
		"BASE_GREATPOWER_DAILY_INFLUENCE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(base_greatpower_daily_influence)),
		"AI_SUPPORT_REFORM", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(ai_support_reform)),
		"BASE_MONTHLY_DIPLOPOINTS", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(base_monthly_diplopoints)),
		"DIPLOMAT_TRAVEL_TIME", ONE_EXACTLY, expect_days(assign_variable_callback(diplomat_travel_duration)),
		"PROVINCE_OVERSEAS_PENALTY", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(province_overseas_penalty)),
		"NONCORE_TAX_PENALTY", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(noncore_tax_penalty)),
		"BASE_TARIFF_EFFICIENCY", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(base_tariff_efficiency)),
		"COLONY_FORMED_PRESTIGE", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(colony_formed_prestige)),
		"CREATED_CB_VALID_TIME", ONE_EXACTLY, expect_months(assign_variable_callback(created_cb_valid_time)),
		"LOYALTY_BOOST_ON_PARTY_WIN", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(loyalty_boost_on_party_win)),
		"MOVEMENT_RADICALISM_BASE", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(movement_radicalism_base)),
		"MOVEMENT_RADICALISM_PASSED_REFORM_EFFECT", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(movement_radicalism_passed_reform_effect)),
		"MOVEMENT_RADICALISM_NATIONALISM_FACTOR", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(movement_radicalism_nationalism_factor)),
		"SUPPRESSION_POINTS_GAIN_BASE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(suppression_points_gain_base)),
		"SUPPRESS_BUREAUCRAT_FACTOR", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(suppress_bureaucrat_factor)),
		"WRONG_REFORM_MILITANCY_IMPACT", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(wrong_reform_militancy_impact)),
		"SUPPRESSION_RADICALISATION_HIT", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(suppression_radicalisation_hit)),
		"INVESTMENT_SCORE_FACTOR", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(country_investment_industrial_score_factor)),
		"UNCIV_TECH_SPREAD_MAX", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(unciv_tech_spread_max)),
		"UNCIV_TECH_SPREAD_MIN", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(unciv_tech_spread_min)),
		"MIN_DELAY_BETWEEN_REFORMS", ONE_EXACTLY, expect_months(assign_variable_callback(min_delay_duration_between_reforms)),
		"ECONOMIC_REFORM_UH_FACTOR", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(economic_reform_uh_factor)),
		"MILITARY_REFORM_UH_FACTOR", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(military_reform_uh_factor)),
		"WRONG_REFORM_RADICAL_IMPACT", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(wrong_reform_radical_impact)),
		"TECH_YEAR_SPAN", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(tech_year_span)),
		"TECH_FACTOR_VASSAL", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(tech_factor_vassal)),
		"MAX_SUPPRESSION", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(max_suppression)),
		"PRESTIGE_HIT_ON_BREAK_COUNTRY", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(prestige_hit_on_break_country)),
		"MIN_MOBILIZE_LIMIT", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(min_mobilize_limit)),
		"POP_GROWTH_COUNTRY_CACHE_DAYS", ONE_EXACTLY, expect_days(assign_variable_callback(pop_growth_country_cache_days)),
		"NEWSPAPER_PRINTING_FREQUENCY", ONE_EXACTLY, expect_days(assign_variable_callback(newspaper_printing_frequency)),
		"NEWSPAPER_TIMEOUT_PERIOD", ONE_EXACTLY, expect_days(assign_variable_callback(newspaper_timeout_period)),
		"NEWSPAPER_MAX_TENSION", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(newspaper_max_tension)),
		"NAVAL_BASE_SUPPLY_SCORE_BASE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(naval_base_supply_score_base)),
		"NAVAL_BASE_SUPPLY_SCORE_EMPTY", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(naval_base_supply_score_empty)),
		"NAVAL_BASE_NON_CORE_SUPPLY_SCORE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(naval_base_non_core_supply_score)),
		"COLONIAL_POINTS_FROM_SUPPLY_FACTOR", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonial_points_from_supply_factor)),
		"COLONIAL_POINTS_FOR_NON_CORE_BASE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonial_points_for_non_core_base)),
		"MOBILIZATION_SPEED_BASE", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(mobilization_speed_base)),
		"MOBILIZATION_SPEED_RAILS_MULT", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(mobilization_speed_rails_mult)),
		"COLONIZATION_INTEREST_LEAD", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(colonization_interest_lead)),
		"COLONIZATION_INFLUENCE_LEAD", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(colonization_influence_lead)),
		"COLONIZATION_MONTHS_TO_COLONIZE", ONE_EXACTLY, expect_months(assign_variable_callback(colonization_duration)),
		"COLONIZATION_DAYS_BETWEEN_INVESTMENT", ONE_EXACTLY,
			expect_days(assign_variable_callback(colonization_days_between_investment)),
		"COLONIZATION_DAYS_FOR_INITIAL_INVESTMENT", ONE_EXACTLY,
			expect_days(assign_variable_callback(colonization_days_for_initial_investment)),
		"COLONIZATION_PROTECTORATE_PROVINCE_MAINTAINANCE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_protectorate_province_maintainance)),
		"COLONIZATION_COLONY_PROVINCE_MAINTAINANCE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_colony_province_maintainance)),
		"COLONIZATION_COLONY_INDUSTRY_MAINTAINANCE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_colony_industry_maintainance)),
		"COLONIZATION_COLONY_RAILWAY_MAINTAINANCE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_colony_railway_maintainance)),
		"COLONIZATION_INTEREST_COST_INITIAL", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_interest_cost_initial)),
		"COLONIZATION_INTEREST_COST_NEIGHBOR_MODIFIER", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_interest_cost_neighbor_modifier)),
		"COLONIZATION_INTEREST_COST", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(colonization_interest_cost)),
		"COLONIZATION_INFLUENCE_COST", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(colonization_influence_cost)),
		"COLONIZATION_EXTRA_GUARD_COST", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_extra_guard_cost)),
		"COLONIZATION_RELEASE_DOMINION_COST", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_release_dominion_cost)),
		"COLONIZATION_CREATE_STATE_COST", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_create_state_cost)),
		"COLONIZATION_CREATE_PROTECTORATE_COST", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_create_protectorate_cost)),
		"COLONIZATION_CREATE_COLONY_COST", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_create_colony_cost)),
		"COLONIZATION_COLONY_STATE_DISTANCE", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_colony_state_distance)),
		"COLONIZATION_INFLUENCE_TEMPERATURE_PER_DAY", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_influence_temperature_per_day)),
		"COLONIZATION_INFLUENCE_TEMPERATURE_PER_LEVEL", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(colonization_influence_temperature_per_level)),
		"PARTY_LOYALTY_HIT_ON_WAR_LOSS", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(party_loyalty_hit_on_war_loss)),
		"RESEARCH_POINTS_ON_CONQUER_MULT", ONE_EXACTLY,
			expect_fixed_point(assign_variable_callback(research_points_on_conquer_mult)),
		"MAX_RESEARCH_POINTS", ONE_EXACTLY, expect_fixed_point(assign_variable_callback(max_research_points))
	);
}