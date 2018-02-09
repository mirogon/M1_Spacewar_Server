# Welcome to M1_Spacewar

## Description
This is the server for [M1_Spacewar](https://github.com/mirogon/M1_Spacewar).  

## Table of Contents
* [How to host your own server](#how-to-host-your-own-server)
* [Build](#build)
* [Credits](#credits)
* [License](#license)

## How to host your own server

1. Forward the port 55555 in your router
2. Download the server build [M1_Spacewar_Server.zip](/build/M1_Spacewar_Server.zip)
3. The clients have to change the server ip to your ip in the config.cfg
4. Start the server, the clients can now connect

## Build
To build this project on your own, you need the following libraries:

* WINDOWS x86

	* [SDL2](https://www.libsdl.org/download-2.0.php)
	* [SDL_ttf](https://www.libsdl.org/projects/SDL_ttf/)
	* [SDL_image](https://www.libsdl.org/projects/SDL_image/)
	* [SDL_net](https://www.libsdl.org/projects/SDL_net/)
	* [Boost](http://www.boost.org/doc/libs/1_66_0/more/getting_started/windows.html)
	* [M1_Random](/lib/M1Random/)
	* [M1_Timer](/lib/M1Timer/)

	
	
## Credits

This project uses the SDL2 libraries provided by libsdl.org.  
M1_Spacewar is developed by [Marcel Römer](https://github.com/mirogon).

## License

M1_Spacewar uses the SDL2 game engine, which is available under the [zlib license](http://www.gzip.org/zlib/zlib_license.html).
This project itself is available under the MIT License

MIT License

Copyright (c) 2018 Marcel Römer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.