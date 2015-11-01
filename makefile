project=comx
objects= App.o comx.o init.o Opreat.o photo.o PIDAndPicture.o menu.o
CFLAGS = -O2
$(project):$(objects)
	gcc -o $@ $^
.c.o:
	gcc -c $< $(CFLAGS)
App.o:common.h comx.h
init.o:common.h comx.h
Opreat.o:common.h comx.h
comx.o:common.h
photo.o:common.h Picture.h
PIDandPicture.o:common.h Picture.h comx.h
menu.o:common.h comx.h App.h
clean:
	rm -rf $(objects)
