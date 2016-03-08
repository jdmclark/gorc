NAME:=client

DEPENDENCIES:= \
	game \

SOURCES:= \
	application.cpp \
	clear_screen_view.cpp \
	hud_view.cpp \
	main.cpp \
	places/action/action_presenter.cpp \
	places/action/action_view.cpp \
	presenter_mapper.cpp \
	world/level_shader.cpp \
	world/level_view.cpp \
	world/pov_mesh_node_visitor.cpp \
	world/thing_mesh_node_visitor.cpp \

LIBRARIES:=$(SFML_LIBRARIES) $(SYSTEM_LIBRARIES)
