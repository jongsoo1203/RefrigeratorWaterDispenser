<<<<<< RelayWaterDispenser
Collaboration: Lorenzo, Jongsoo, Minh, Max

# Smart Refrigerator Water Dispenser

Schematics, code, Web-app and 3D files all available.

Visit this for the WebApp [GitHub repository](https://github.com/loty2019/water-dispenser-web).

## Overview
This project aims to automate the refrigerator water dispenser. Originally located inside the fridge, it has been moved outside using a custom 3D-designed fountain. We first prototyped it on Arduino and then transitioned to the ESP32 platform with internet capabilities. The entire system now operates on a real-time database hosted on Firebase.

## Purpose (Why should this exist?)

The fridge has an internal water dispensing system that is manual. You have to hold the water bottle up, then at the same time press a hard button. In this process, the refrigerator loses coldness, and you get cold. Our system, however, utilizes an external custom-designed fountain that provides cold water after just 2 seconds of water flowing with no need to hold or press anything.

## Features

### 1. Remote Control

Our system reads signals from a remote controller, a simple numeric keypad used to navigate menus, set parameters, and choose which water bottle to fill and how.

### 2. Display

The 1602 display with I2C protocol provides real-time information.

### 3. Motion Activation

We've integrated a motion sensor that wakes the device from deep sleep only when someone approaches for water, enhancing energy efficiency.

### 4. Web App

We chose to use Next.js and Tailwind CSS to develop a simple yet useful UI to keep track of the water consumption of each user.

### 5. Custom Feature

Hosting guests or following a recipe that demands an exact amount of water? Input the desired ounces, enable unlimited fill, and so on... a handy extra feature for any occasion.

### 6. Relay

To communicate with the fridge on when to open the flow, we decided to use a relay since it allows handling 120V currents with just a 3.3V board.

### 7. Internet

Everything from water consumption to settings will be stored on a Firebase real-time database.

## Settings, Error Handling, and Details

We calculated the dispenser's flow rate and programmed the ESP to adjust the dispensing time accordingly. The user can also adjust it, and everything will be stored on the Flash memory and the database.

### Error Documentation:
Error #01 = Failed to record individual water consumption.

Error #02 = Failed to push consumption to the database.

Error #03 = Failed to generate date or hour due to connection issues.

Others coming soon...

### How to Build (soldering might be required)

- Follow the schematics and picture attached below (these are for the prototype) if you are not using the I2C protocol to cominicate with the display then use a NPN transistor to turn off the screen.
  I actually used a 3 pin relay so the image below is not accurate
<img width="847" alt="Screenshot 2023-12-01 at 2 03 21 AM" src="https://github.com/loty2019/RefrigeratorWaterDispenser/assets/125715080/e65917bb-6f01-427a-8e4f-efa38740c8e7">
[Water Dispenser.pdf](https://github.com/loty2019/RefrigeratorWaterDispenser/files/13521786/Water.Dispenser.pdf)

- 3D printable files are in STL_files folder
![3D design](https://github.com/loty2019/RefrigeratorWaterDispenser/assets/125715080/bef2b874-d871-40e2-90d2-5a8ef3ee35ca)

- Set up a database
  
- FINAL PRODUCT (you can build a case for it but I like to see it exposed lol)

=======
# Smart Refrigerator Water Dispenser

Are your mornings a rush, and those precious 40 seconds spent at the fridge seem like an eternity? Instead of sacrificing more sleep, why not make life easier? Enter the Smart Refrigerator Water Dispenser project!

## Overview

This project aims to automate the process of using a refrigerator water dispenser, saving you time and effort. By integrating an Arduino with the refrigerator, we've transformed a mundane task into a seamless experience. The Arduino is connected to both the water dispensing system and a remote controller.

## Features

### 1. Remote Control Convenience

Say goodbye to manual dispensing. Our system reads signals from a remote controller, a simple numeric keypad. Each number corresponds to a specific water bottle, determining the precise amount of water to dispense.

### 2. Intelligent Display

The 1602 display provides real-time information on the dispensed water, ensuring clarity and convenience. No more guessing – the display shows the selected bottle size and dispensed ounces.

### 3. Motion-Activated Display

Worried about having the display on all the time? Fear not! We've integrated a motion sensor connected to an NPN transistor. The display activates only when someone approaches for water, enhancing energy efficiency.

### 4. Custom Filling Feature

Hosting guests or following a recipe that demands an exact amount of water? Our system has you covered. Input the desired ounces, and watch as the dispenser accurately delivers the specified amount – a handy extra feature for any occasion.

## Technical Details

To ensure precision, we calculated the dispenser's flow rate and programmed the Arduino to adjust the dispensing time accordingly. This collaboration between Lorenzo and Jongsoo brings together expertise in electronics and automation.

Make your mornings smoother, and add a touch of tech to your kitchen with the Smart Refrigerator Water Dispenser!

**Collaboration:** Lorenzo and Jongsoo
