##
# Project Title
#
# @file
# @version 0.1

-include tracker.d
tracker: tracker.c
	gcc -MMD tracker.c -lSDL2 -lm -o tracker

run: tracker
	./tracker
# end
