# SFPC

A strategy RPG game engine for Windows PCs written in C++, inspired by the Shining series of games for the Genesis.


## History

SFPC has been in on again, off again development since the early 2000s.  The first version was built with SDL, but did not get very far.  The second revision used DirectDraw and accomplished more, but was also eventually abandoned.  This third attempt which started back up around 2009 uses very basic OpenGL and while not complete it faithfully recreates much of the look and feel of the original Shining games.  It has a window system, its own basic scripting system, an in-game level editor for quick iteration, and the beginnings of a functional combat system.

SFPC was the product of a very stubborn and determined beginner trying to learn about making games by doing it the long way - writing an engine in C++ from the ground up.  I had the good fortune of having a wonderful mentor, Antonio Maiorano, who did a fantastic job of teaching me the right things at the right times, kept me motivated, and ultimately taught me how to teach myself.  While I got a lot of things right, I did many other things inefficiently or even downright backwards in places, and if I was to do it all over I would do many things much differently.  Three times was enough though, a fourth do-over seems very unlikely.

I'm releasing this now for others to take from it what they will, be they a Shining fan who wants to see how I did what I did with my engine, or other aspiring developers who want to see the insides of a homebrew C++ game engine.  There's a possibility I may return to the project and tinker with it some more, but no promises.


## Developer Setup

The bulk of development on SFPC was done in Visual Studio 2008 Express, but has since been upgraded to Visual Studio 2010.  I expect it would likely build fine in newer versions but that may take a little tweaking.

The current build is frozen in time as it was when I last worked on it during the summer of 2012 while implementing basic enemy pathfinding.


## Links
I documented much of my development process on my blog at http://kenhilfportfolio.blogspot.com/search/label/SFPC where you can read more and see some demo videos I put together along the way.  