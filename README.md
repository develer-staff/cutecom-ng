# Cutecom-ng #

Cutecom-ng is a graphical serial port terminal running on Linux/Mac OSX and
Windows, written in Qt5


## Features

 - a readline-like history of sent commands
 - splittable terminal window for easy output browsing
 - a handy search feature
 - configurable end of line char
 - binary or text-mode dump file
 - XModem File transfer
 - more to come... contributions welcome ;-)

## Installation

**download the zip**

**OR**

**build the release yourself**

 1. download / install Qt5 and the QSerialPort add-on

 2. clone the repository  
 ```git clone https://github.com/develersrl/cutecom-ng```

 3. open ./cutecom-ng/cutecom-ng.pro with QtCreator

## Usage


### Serial port emulation

you can easily emulate a serial port with **gnu-screen**

 1. download and install **gnu-screen**

 (for linux distros with apt package management)  
 ```sudo apt-get install screen```  

 2. if your port is `/dev/ttyUSB0`, you do `screen /dev/ttyUSB0 115200` to open
the port at the specified baudsrate


### Null-modem cable emulation example

To emulate a null-modem connection, you can just use **socat** and
**gnu-screen**.

In our example, we will transmit a file with **Cutecom-ng**, using the XModem
protocol, to a virtual serial port opened with socat. **screen** and **rx**
handling the reception of the file.

 1. install socat, gnu-screen and lrzsz packages:
```sudo apt-get install socat screen lrzsz``` on ubuntu and other **apt** based
linux distros

 2. open 2 terminals

 3. in the 1st terminal, run:

```socat -d -d pty,raw,echo=1 pty,raw,echo=1```

basically we tell `socat` to create 2 virtual ports and to open them
```
2015/07/10 09:36:44 socat[18188] N PTY is /dev/pts/12
2015/07/10 09:36:44 socat[18188] N PTY is /dev/pts/16
2015/07/10 09:36:44 socat[18188] N starting data transfer loop with FDs [3,3] and [5,5]
```
this is the kind of output that you should have.

In our example **socat** opened
`/dev/pts/12` and `/dev/pts/16`. Now we choose arbitrarily to use **/dev/pts/12**
for transmission, **/dev/pts/16** for reception.

 4. in the 2nd terminal, run:

```screen /dev/pts/16 115200```

this opens a **screen** session on `/dev/pts/16` with a baudsrate of 115200.
Once inside **screen**, type *[CTRL-A]* followed by the `:` character. This will
open a prompt at the bottom left of the **screen** window, write now the
following command but don't run it:

```exec !! rx filename```
a file called `filename` will be created in the current directory, containing
the bytes received.

5. open port ```/dev/pts/12``` in Cutecom-ng, with a baudsrate of 115200 and the
default connection settings

6. Click on XModem and choose a file to send. After having clicked to start the
transmission, come back to the previous **gnu-screen** terminal and execute the
command

7. Enjoy the speed of an XModem transmlission

If all goes well, a copy of `myfile` named `filename` should be created in the
directory you were when you opened the screen session. Careful, if filename
exists it will be overwritten


## Contribute

All contributions are welcome!

### Issues

you can create issues
 [here](https://github.com/develersrl/cutecom-ng/issues/new)

but before doing that,
 - please check [there](https://github.com/develersrl/cutecom-ng/issues) that a
 similar issue doesn't already exist
 - include as many details as you can possibly get,
 - a reproducible test case would be perfect

### Pull-requests:

#### git commit messages
- use the present tense ("Add feature" not "Added feature")
- use imperative ("Close serial port" not "Closes serial port")
- limit the first line to 72 characters or less

#### coding style
- keep the same coding style as the file in which you are adding/removing code
(tabs/spaces, indentation, etc.)
- if your pull-request adds a new source file, it should have the same coding
style as `MainWindow.cpp`
- end of line are linux-style

## Credits

People and project deserving credits are listed in... [CREDITS](./CREDITS)

## License

Cutecom-ng is released under the GPL3, see [LICENSE](./LICENSE)
