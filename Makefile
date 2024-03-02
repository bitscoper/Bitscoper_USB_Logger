all: Bitscoper_USB_Logger

Bitscoper_USB_Logger: Bitscoper_USB_Logger.c
	gcc ./Bitscoper_USB_Logger.c -o ./Bitscoper_USB_Logger -ludev

clean:
	rm -f Bitscoper_USB_Logger
