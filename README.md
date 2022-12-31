# SmartSolarBoxMini

Smart Solar Box is a smart and intelligent solar power storage unit with monitoring and control capabilities.This is an education project designed for kids and students to learn about Solar photovoltaic energy and battery technology. The smart solar box is composed of a power system and an information system. It has four main elements: SSB mini PCB, ESP8266 D1 Mini, a solar panel, and a battery. 

![ssb](IMG_SSB_mini.jpg)

The solar panel absorbs solar radiation and turns it into direct-current electricity. SSB mini PCB consists of Battery charger IC that takes care of battery charging process. Boost converter converts battery voltage to 5V regulated output. INA3221 power monitoring IC monitors the power flow. ESP D1 mini communicates with SSB Mini PCB and runs the Solar Box live dashboard. D1 mini get the power from SSB mini pcb power line. ESP mini creates its own wifi hotspot so that smart phones/laptops could be connected and access the dashboard easily. 

![ssb Architecture](SSB_mini_architecture.jpg)

This project aims at kids and students to learn about solar energy and battery storage technologies. Using the live monitoring dashboard, students can observe the power flow and readings. Activity and experiment sheets help students learn about various concepts related to sustainable energy, battery storage and solar energy.

![ssb portal](SSB_mini_portal.jpg)

- Resources for pcb manufacturing and other required parts are present in "Hardware" folder



