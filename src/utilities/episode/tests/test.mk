include ../../../../../rules/test.mk

EPISODE:=$(BIN)/episode

simple-episode: \
	parse-episode \
	process-raw-output \
	compare-output

parse-episode:
	-$(EPISODE) <episode.jk >$(RAW_OUTPUT) 2>&1
