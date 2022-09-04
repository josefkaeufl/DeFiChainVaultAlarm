# DeFiChainVaultAlarm
Checks your vault status and generates an alarm when it falls below a defined threshold.

## Prepare Arduino IDE

### Download and install Arduino IDE
[download](https://www.arduino.cc/en/software)

### Install ESP32 board
Arduinon -> Tools -> Board -> Board Manager -> esp32 (v 2.0.3 was used)

### Install UniversalTelegramBot
Arduino -> Tools -> Manage Libraries -> UniversalTelegramBot (v 1.3.0 was used)

### Install Arduino JSON
Arduino -> Tools -> Manage Libraries -> ArduinoJson (v 6.18.0 was used)
2
### Install Analog Write
Arduino -> Tools -> Manage Libraries -> ESP32 AnalogWrite (v 0.1.0 was used)

### Install LiliGo library
Download zip from https://github.com/Xinyuan-LilyGO/LilyGo-EPD47
-> https://github.com/Xinyuan-LilyGO/LilyGo-EPD47/archive/refs/heads/master.zip
Install it:
Arduino -> Sketch -> Include Library -> Add .ZIP Library -> select downlod from LiliGo library


## Configure Code
Open ino file in Arduino

### Wifi connection
Add your wifi details in p_ssid and p_password

### DeFiChain Vault
Add your vault address in p_link

### Telegram bot
- search for the user "botfather" in your telegram client
- send him the message "/start"
- send "/newbot" to create a new bot
- follow the instructions in order to create the bot and get the bot token
- add the bot token to BOTtoken in your ino file

### Telegram user ID
It may make sense to add your user ID to the ino file. 
- search for the user "IDBot" in ypur telegram client
- send "/getid"
- add your user ID to CHAT_ID in your ino file

## Bugfixes

### Java2 vs. Java3
In case you get some compile error like this "exec: "python": executable file not found in $PATH" then follow the instructions from here:
https://georgik.rocks/esp32-arduino-macos-exec-python-executable-file-not-found-in-path/
Note: Your Arduino15 folder can be located somewhere else. Please check the prefrences of your Arduino installation.


## Hardware

### ESP32 Lilygo
You will need this hardware:
[The Lilygo ESP32 e-Paper display (PH 2.0 variant)](https://de.aliexpress.com/item/1005002006058892.html?spm=a2g0o.order_list.0.0.56495c5fTINk9D&gatewayAdapt=glo2deu), 
[a Battery (example)](https://de.aliexpress.com/item/1005002919536938.html?spm=a2g0o.productlist.0.0.74036fa6idQD2W&ad_pvid=202205021227217858146889041840000671022_1&s=p) and 
[the cable able for buzzer](https://de.aliexpress.com/item/1005003912905288.html?spm=a2g0o.order_list.0.0.56495c5fTINk9D&gatewayAdapt=glo2deu)

### Buzzer
I bougth this [buzzer](https://www.amazon.de/gp/product/B07DPR4BTN/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)

### Connect the Buzzer
In my example the buzzer GND pin is directly connected to the uC ground and the supply pin (+) is connected to D0 of the uC board.


## Compile

- open the ino file
- make sure you selected the "ESP32 Dev Module" board in Skech > Tools > Board...
- set Skech > Tools > PSRAM to enabled
- connect the board to your computer
- select the new port in Sketch > Tools > Port...
- upload the file

