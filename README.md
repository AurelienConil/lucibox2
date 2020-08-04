# lucibox2

new version of lucibox software

#Software to run

Lucibox is based on pure data and use CEAMMC lib.
2 options to use it :

1) Use standard puredata ( http://puredata.info/ ) and add the CEAMMC lib as externals lib
2) Use the CEAMMC version of puredata ( https://github.com/uliss/pure-data/releases/tag/v2020.07 )

#Example folder

Different examples that works only on computer, to test and understood how lucibox works

#Machine folder

Each machine is supposed to be a hardware machine, running the pd patch on raspberry pi , and interfacing with an arduino controller.
At least, each machine works also directly on computer.

Note that, is each /machine/xxx/ folder there is 2 pd patch.
1) nogui.pd contains only the audio part
2) machine.pd contains the GUI part AND the nogui.pd 

So if you want to run it on your computer, run machine.pd
if you want to run it on a raspberry pi headless with no gui, run nogui.pd

