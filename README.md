# Smart Refrigerator Water Dispenser

Are your mornings a rush, and those precious 40 seconds spent at the fridge seem like an eternity? Instead of sacrificing more sleep, why not make life easier? Enter the Smart Refrigerator Water Dispenser project!
Schematics, code instructions, and 3D (stl for 3d printer) files

## Overview

This project aims to automate the process of using a refrigerator water dispenser, saving you time and effort. By integrating an Arduino with the refrigerator, we've transformed a mundane task into a seamless experience. 

## Features

### 1. Remote Control Convenience

Our system reads signals from a remote controller, a simple numeric keypad. Each number corresponds to a specific water bottle, determining the precise amount of water to dispense.

### 2. Intelligent Display

The 1602 display provides real-time information on the dispensed water, ensuring clarity and convenience. The display shows the selected bottle size and dispensed ounces.

### 3. Motion-Activated Display

Worried about having the display on all the time? Fear not! We've integrated a motion sensor connected to an NPN transistor. The display activates only when someone approaches for water, enhancing energy efficiency.

### 4. Custom Filling Feature

Hosting guests or following a recipe that demands an exact amount of water? Input the desired ounces, and watch as the dispenser accurately delivers the specified amount – a handy extra feature for any occasion.

## Technical Details

To ensure precision, we calculated the dispenser's flow rate and programmed the Arduino to adjust the dispensing time accordingly.

Make your mornings smoother, and add a touch of tech to your kitchen with the Smart Refrigerator Water Dispenser!

### How to Run
Download the file WaterDispenser/WaterDispenser.ino load that into Arduino IDE and then upload it to the Arduino UNO.

### How to Build
-Follow the schematics and picture attached below
-3D printable files are in STL_files folder
![3D design](https://github.com/loty2019/RefrigeratorWaterDispenser/assets/125715080/bef2b874-d871-40e2-90d2-5a8ef3ee35ca)

**Collaboration:** Lorenzo and Jongsoo
