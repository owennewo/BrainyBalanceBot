# BrainyBalanceBot

This BrainyBalanceBot is not yet brainy!  

I'm separtely working on some Machine Vision code (see MaixPy_scripts april tags) which will be 
running on a Maix Dock (microcontroller with OpenMV support) which will sit atop the balance bot.

This repo is designed to run on a nano iot 33 which is a samd21 microcontroller with esp32 side chip
for wifi/bluetooth.

As both Maix Dock and nano iot have wifi - I intend to setup the nano iot as wifi access point and get
the brains to send messages over UDP.

Example - Maix Dock detects april tag #1 which is 'friendly' - Maix Dock sends coordinates to nano to follow.  Similarly 
april tag #2 is deemed hostile and nano avoids this.  April tags are very easy to classify, perhaps I can program my cat Henry
as a friend or foe!?
