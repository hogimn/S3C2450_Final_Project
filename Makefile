all:
	make -C ./src/
clean:
	make -C ./src/ clean
deploy:
	make -C ./src/ deploy
