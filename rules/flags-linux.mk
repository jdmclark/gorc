ifndef FLAGS_INCLUDED
$(error flags.mk must be included before this file)
endif

DEFAULT_LIBRARIES:= \
	-lboost_system \
	-lboost_filesystem \
	-lboost_thread \

SFML_LIBRARIES:= \
	-lGLEW \
	-lGL \
	-lGLU \
	-lsfml-audio \
	-lsfml-graphics \
	-lsfml-window \
	-lsfml-system \
