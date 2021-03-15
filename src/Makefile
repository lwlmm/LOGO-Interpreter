G=gcc
ROUTINE= -fsanitize=undefined -fsanitize=address -g3
ULTRA= -Wall -Wextra -Wfloat-equal -pedantic -std=c90 -Werror -O3 $(ROUTINE) $(SDL)
SDL= `sdl2-config --cflags` `sdl2-config --libs` -lm

#An example of the simplest parser
#Related file: 01.no
example : example.c
	$(G) example.c -o example $(ROUTINE)

xexample:
	rm -f example

#Basic parser to the simplified grammar
#Related file: 1.ttl 2.ttl 3.ttl 4.ttl
parser : parser.c parser.h
	$(G) parser.c -o parser $(ROUTINE)

xparser :
	rm -f parser

#Interpretor to the simplified grammar
#Related file: 1.ttl 2.ttl 3.ttl 4.ttl

interp : newtestinter.c parser.h mysdl.h
	$(G) newtestinter.c -o interpretor  $(SDL)

xinterp :
	rm -f interpretor

#Some test to mysdl.h file
newtest : testnewh.c mysdl.h
	$(G) testnewh.c -o testnew $(SDL)

xnewtest :
	rm -f testnew

#Extension Interpretor with whole turtle grammar parser and Rainbow color
#Related file: whole.ttl whole1.ttl whole2.ttl whole3.ttl whole4.ttl whole5.ttl
extension : extension.c mysdl.h
	$(G) extension.c -o extension $(SDL) $(ULTRA)

xextension :
	rm -f extension