# DeFiChainVaultAlarm
Checks your vault status and generates an alarm when it falls below a defined threshold.

## Prepare Arduino IDE

### Download and install Arduino IDE
https://www.arduino.cc/en/software


### Install ESP8266 libs
Follow instructions from here https://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/

### Install Telegram bot lib
- download https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/archive/master.zip
- Go to Sketch > Include Library > Add.ZIP Library...
- Add the library you’ve just downloaded.

### Install ArduinoJson lib
- Go to Skech > Include Library > Manage Libraries.
- Search for “ArduinoJson”.
- Install the library.

## Configure code
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


## Hardware

### ESP8266
You will need something like this [Amazon](https://www.amazon.de/AZDelivery-NodeMCU-ESP8266-ESP-12E-Development/dp/B06Y1ZPNMS/ref=asc_df_B06Y1ZPNMS/?tag=googshopde-21&linkCode=df0&hvadid=310638483583&hvpos=&hvnetw=g&hvrand=16580639070590533171&hvpone=&hvptwo=&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9042673&hvtargid=pla-378483937875&psc=1&th=1&psc=1&tag=&ref=&adgrpid=63367893073&hvpone=&hvptwo=&hvadid=310638483583&hvpos=&hvnetw=g&hvrand=16580639070590533171&hvqmt=&hvdev=c&hvdvcmdl=&hvlocint=&hvlocphy=9042673&hvtargid=pla-378483937875)
or an ESP32 but in this case the code might need some adaptions

### Buzzer
I used this [buzzer]
(https://www.amazon.de/gp/product/B07DPR4BTN/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1)

### Breadboard
Maybe you will need a breadboard and some wires if you do not want to solder.

### Connect the Buzzer
In my example the buzzer GND pin is directly connected to the uC ground and the supply pin (+) is connected to D0 of the uC board.


## Compile

- open the ino file
- make sure you selected the ESP8266 board in Skech > Tools > Board...
- connect the board to your computer
- select the new port in Sketch > Tools > Port...
- upload the file

## Sorry...
for this quick and dirty howto
