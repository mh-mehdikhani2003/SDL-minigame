# SDL-minigame
head football. The C++ project for BP course of electrical engineering department of Sharif university of technology.

## Installation
at first you should install SDL2, SDL2_gfx, SDL2_image, SDL2_ttf.
in debian based distributions this command will do that:  
`sudo apt install libsdl2*`  
also you will need g++ as compiler. You can install it by runnig this:  
`sudo apt install g++`

## Making
There is a shell file named make.sh in the project root directory which help you with making the game.
Just try these commands on the project root directory:  
`sudo chmod +x make.sh`  
`./make.sh`  
the above commands make "Program.out" which is the GAME itself!

## Playing!
Run the game with the following command:  
`./Program.out`  

after selecting names, characters and ball the game will start.  
there are two characters which are controlled using 'w s a d' and arrow keys.  
also, each character has a special power which is shown below its names (top of the screen).  
the powers can be enabled by pressing 's' or bottom arrow. it is obvious that power bars(below each goal) must be complete.  

you have 90 seconds. if any player scores more than 10 the game will finish!

Enjoy dude!
