ifeq ($(LIBUSB_1_0),)
PACKAGES=libusb
else
PACKAGES=libusb-1.0
CFLAGS=-DLIBUSB_1_0
endif

CFLAGS+=-Wall `pkg-config --cflags ${PACKAGES}`
LIBS=`pkg-config --libs ${PACKAGES}`

MAINSRC=main.c
OBJS=main.o

all: dcdc_nuc_list

.c.o:
	$(CC) $(CFLAGS) $(INCS) -c $(MAINSRC)

dcdc_nuc_list: $(MAINSRC)
	$(CC) $(CFLAGS) $(MAINSRC) -o $@ $(LIBS) $(INCS)

install:
	cp dcdc_nuc_list /usr/bin

clean:
	rm -rf *.o dcdc_nuc_list
