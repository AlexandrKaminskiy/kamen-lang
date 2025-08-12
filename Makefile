compile:
	sh ./build.sh
run:
	sh ./run.sh $(word 2, $(MAKECMDGOALS))
clean:
	sh ./clean.sh