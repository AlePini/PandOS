<!-- PROJECT LOGO -->
<br />
<p align="center">
    <img src="images/logo.png" alt="Logo" width="100" height="100">
  <p align="center">
  </p>
  <p align=center>
    <img src="https://img.shields.io/badge/version-1.0-blue" alt="version">
    <a href="LICENSE">
      <img src="https://img.shields.io/github/license/virtualsquare/umps3" alt="License">
    </a>
  </p>
  <p align=center>
    <img src="https://img.shields.io/badge/phase-1-brightgreen" alt="version">
  </p>
</p>


## PandOS
The PandOS operating system is an educational project consisting in the 
implementation of a kernel/OS designed to run on µMPS.

## How to Run this Project
Currently there is no pakcage, you need to install it from source

### Dependency
- umps3
- cmake
- make

### Compile
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

**If you have umps3 library installed in a weird location you can use this instead of "$ cmake .."**
```
$ cmake .. -DUMPS3_DIR_PREFIX=path
```
Also you can set *UMPS3_INCLUDE_DIR*, *UMPS3_DATA_DIR* and *UMPS3_LIB_DIR* individually.

### Launch
* Follow the instructions above to compile the project.  
* Then open umps3 and create a new Machine.  
* Replace "Core File" with build/PandOS.core.umps and "Symbol Table" PandOS.stab.umps
* Fianlly, click on "PowerON" and "Continue" to launch

## Credits
Mattia Biondi and Renzo Davoli for [µMPS3](https://github.com/virtualsquare/umps3)  
Tomislav Jonjic for the original [µMPS](https://github.com/tjonjic/umps)

## License
This project is licensed under the terms of the [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.en.html license. [See LICENSE for more information](LICENSE).

µMPS3 is licensed under the [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) license. [See LICENSE for more information](https://github.com/virtualsquare/umps3/blob/master/LICENSE).
