all:
	mkdir -p ./obj/
	mkdir -p ./bin/
	g++ -m32 -std=c++11 -c -I./inc/ -I/usr/local/include  ./src/*.cpp
	mv *.o obj/
	ar rv -o ./bin/squirrel.a ./obj/*.o
	cp bin/squirrel.a ../kIRCBot/lib/
	@echo done.

clean:
	rm -Rf bin/*
	rm -Rf obj/*
	rm -Rf ../kIRCBot/lib/squirrel.a
