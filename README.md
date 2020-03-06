# BrainyBalanceBot

This BrainyBalanceBot is not yet brainy!  

I'm separtely working on some Machine Vision code (see separate MaixPy_scripts repo and april tags script) which will be 
running on a Maix Dock (microcontroller with OpenMV support) which will sit atop the balance bot.

This repo is designed to run on a nano iot 33 which is a samd21 microcontroller with esp32 side chip
for wifi/bluetooth.

As both Maix Dock and nano iot have wifi - I intend to setup the nano iot as wifi access point and get
the brains to send messages over UDP.

Example - Maix Dock detects april tag #1 which is 'friendly' - Maix Dock sends coordinates to nano to follow.  Similarly 
april tag #2 is deemed hostile and nano avoids this.  April tags are very easy to classify, perhaps I can program my cat Henry
as a friend or foe!?

There are a couple of other parts to this project (apart from Maix).

There is a p5.js UI that connects over serial USB to allow arduino balance metrics to be viewed and the two PID controllers to be tuned.
https://editor.p5js.org/owennewo/sketches/dnDiWqNw

P5.js is a bit like processing.org but runs in a browser and is javascript based.  

There is also some code that is running on my remote control module - esp32.  This also talks to arduino using same protocol.
I've not committed this, and I've not commited the UDP code yet.

I also have fusion360 files for the 3d printed parts.

This bot makes use of a stepper motor driver board that was originally intended for a cnc machine.  Not sure why I went for this rather than soldering my own!
