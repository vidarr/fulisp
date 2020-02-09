# (c) 2020 michael j. beer
#
# this program is free software; you can redistribute it and/or modify
# it under the terms of the gnu general public license as published by
# the free software foundation; either version 3 of the license, or
# (at your option) any later version.
#
# this program is distributed in the hope that it will be useful,
# but without any warranty; without even the implied warranty of
# merchantability or fitness for a particular purpose.  see the
# gnu general public license for more details.
#
# you should have received a copy of the gnu general public license
# along with this program; if not, write to the free software
# foundation, inc., 51 franklin street, fifth floor, boston, ma  02110-1301,
# usa.

SOURCE_FILES= cons.c error.c expression.c fulispreader.c hash.c  \
			  nativefunctions.c print.c reader.c safety.c streams.c \
			  symboltable.c  test.c environment.c eval.c stack.c lambda.c \
			  benchmarking.c memory.c garbage_collector.c futypes.c fulisp.c \
			  operating_system.c

HEADER_FILES= config.h cons.h error.h expression.h fulispreader.h hash.h \
			  nativefunctions.h print.h reader.h safety.h streams.h \
			  symboltable.h test.h environment.h eval.h stack.h lambda.h  \
			  benchmarking.h memory.h garbage_collector.h futypes.h fulisp.h \
			  operating_system.h
