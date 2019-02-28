#include "application.hpp"
#include "game/world/level_model.hpp"
#include "game/world/level_presenter.hpp"
#include "jk/vfs/gob_virtual_container.hpp"
#include "libold/base/events/print.hpp"
#include "world/level_view.hpp"
#include <boost/algorithm/string/predicate.hpp>

gorc::client::application::application(service_registry const &services)
    : gorc::application<view_layer, presenter, presenter_mapper>("Gorc",
                                                                 mapper,
                                                                 services,
                                                                 make_box(make_vector(0, 0),
                                                                          make_vector(1920, 1080)),
                                                                 true)
    , services(services)
    , mapper(*this)
    , virtual_filesystem(services.get<jk_virtual_file_system>())
    , components(services)
{
    return;
}

gorc::client::application::~application()
{
    return;
}

void gorc::client::application::startup(event_bus &eventbus, content_manager &content)
{
    // Register handler to print messages.
    print_delegate = eventbus.add_handler<events::print>(
        [](const events::print &print) { LOG_INFO(print.message); });

    level_view = std::make_unique<world::level_view>(content, renderer_object_factory);
    action_view = std::make_unique<action::action_view>();

    register_verbs();

    views.set_layer(view_layer::clear_screen, clear_view);

    // HACK: Set current episode to The Force Within.
    {
        diagnostic_context dc(input_episodename.c_str());
        episode_ctr = std::make_unique<gob_virtual_container>(input_episodename);
        virtual_filesystem.set_current_episode(*episode_ctr);
    }

    // HACK: Set current level to 01narshadda.jkl.
    components.services.add_or_replace(eventbus);
    components.services.add_or_replace<gorc::renderer_object_factory>(renderer_object_factory);
    auto contentmanager = std::make_shared<content_manager>(components.services);
    const auto &lev = contentmanager->load<content::assets::level>(input_levelname);

    // Start game:
    components.current_level_presenter = std::make_unique<game::world::level_presenter>(
        components, game::world::level_place(contentmanager, lev));
    place_controller.go_to(action::action_place());

    components.current_level_presenter->start(eventbus);

    level_view->set_presenter(components.current_level_presenter.get());
    level_view->set_level_model(components.current_level_presenter->model.get());

    views.set_layer(view_layer::world, *level_view);

    return;
}

void gorc::client::application::shutdown()
{
    return;
}

void gorc::client::application::update(const gorc::time &time, const box<2, int> &)
{
    components.current_level_presenter->update(time);
}

void gorc::client::application::register_verbs()
{
    auto &verb_table = components.verbs;

    // Register common, general-purpose verbs.
    game::world::level_presenter::register_verbs(verb_table, components);
}

void gorc::client::application::create_options(options &opts)
{
    opts.insert(make_value_option("episode", input_episodename));
    opts.insert(make_value_option("level", input_levelname));

    opts.emplace_constraint<required_option>(std::vector<std::string>{"episode", "level"});
    return;
}
