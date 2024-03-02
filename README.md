# Bitscoper_USB_Logger
Monitors udev USB events and logs device attributes. Works on Linux.

## Compilation
```
gcc ./Bitscoper_USB_Logger.c -o ./Bitscoper_USB_Logger -ludev
```
or
```
make
```

## Usage
```
./Bitscoper_USB_Logger
```

A text file named **USB.log** will be generated in the directory in which the program is running. So, make sure the program or the current user has sufficient permission to do so.  
If it already exists, a confirmation prompt will appear asking whether to overwrite the file or not.