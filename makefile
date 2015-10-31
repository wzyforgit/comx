project=comx
objects= App.o comx.o init.o Opreat.o photo.o PIDAndPicture.o
CFLAGS = -O2
$(project):$(objects)
	gcc -o $@ $^
.c.o:
	gcc -c $< $(CFLAGS)
App.o:comx.h
init.o:comx.h
Opreat.o:comx.h Picture.h
comx.o:App.h
photo.o:Picture.h
PIDandPicture.o:Picture.h comx.h
clean:
	rm -rf $(objects)
