##
# Project Title
#
# @file
# @version 0.1

tracker: tracker.c
	gcc tracker.c -lSDL2 -lm -o tracker

run: tracker
	./tracker
# end
