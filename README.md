<div align="center">

# Bitscoper USB Logger

Monitors udev USB events and logs device attributes. Works on Linux.

[![Release Code](https://github.com/bitscoper/Bitscoper_USB_Logger/actions/workflows/Release_Code.yaml/badge.svg)](https://github.com/bitscoper/Bitscoper_USB_Logger/actions/workflows/Release_Code.yaml)

</div>

## Compilation

### Installing Dependencies

On Fedora,

```
sudo dnf install systemd-devel
```

### Compiling

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
