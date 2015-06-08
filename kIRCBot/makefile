debug:
	mkdir -p ./obj/
	mkdir -p ./bin/
	mkdir -p ./bin/Debug/
	g++ -m32 -pthread -std=c++11 -L./lib/ -c -g -I./inc/ -I/usr/local/include -D_DEBUG ./src/*.cpp
	mv *.o obj/
	g++ -m32 -pthread -std=c++11 -L./lib/ -o ./bin/Debug/kIRCBot ./obj/*.o -L/usr/local/lib ./lib/squirrel.a ./lib/sqstdlib.a /usr/local/lib/libboost_regex.a
	@echo done.

release:
	mkdir -p ./obj/
	mkdir -p ./bin/
	mkdir -p ./bin/Release/
	g++ -m32 -pthread -std=c++11 -L./lib/ -c -I./inc/ -I/usr/local/include  ./src/*.cpp
	mv *.o obj/
	g++ -m32 -pthread -std=c++11 -L./lib/ -o ./bin/Release/kIRCBot ./obj/*.o -L/usr/local/lib ./lib/squirrel.a ./lib/sqstdlib.a /usr/local/lib/libboost_regex.a
	@echo done.

all: debug release

clean:
	@rm -Rf bin/kIRCBot
	@rm -Rf bin/Debug/kIRCBot
	@rm -Rf bin/Release/kIRCBot
	@rm -Rf obj/*