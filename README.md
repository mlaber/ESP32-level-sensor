# ESP32-level-sensor
ESP32 ultrasonic measurement for containers, reservoirs, silos ... with prometheus exporter
# ESP32 Ultraschall-Füllstandssensor 

*Messen von Füllständen und Pegel in Regenwasserzisternen, Pools, Fässern, Container, Silos ... mit Prometheus Exporter und einem Grafana Beispiel-Dashboard.*

## Das Problem

![Failed sensor](/images/old_sensor.jpg)

Nachdem mein kapazitiver [Homematic Füllstandsmesser](https://de.elv.com/elv-homematic-komplettbausatz-kapazitiver-fuellstandsmesser-hm-sen-wa-od-fuer-smart-home-hausautomation-104945) bei einem starken Regen in unserer unterirdischen Zisterne geflutet wurde und ich dass ewige Wechseln der Batterien leid war, habe ich begonnen eine günstige Alternative auf Basis eines ESP32 zu entwickeln. Die Messwerte des Sensors werden via HTTP im [prometheus](https://prometheus.io) exporter format bereitgestellt und können dann bequem archiviert und mit [Grafana](https://grafana.com) visualisierrt werden. 


## Die Hardware

Neben einem ESP32 development board und einem Ulraschallsensor, hier ist der wasserdichte [JSN-SR0T4 Ultraschallsensor](https://www.amazon.de/DollaTek-Wasserdichter-Ultraschall-Abstandssensor-Messwandler-Modul-Arduino/dp/B07PMGJ231) dem günstigeren und nicht geschützen HC-SR04 deutlich überlegen, braucht man noch 2x10 kOhm Wiederstände für den Spannungsteiler um die Eingangsspannung zu überwachen. Wer den ESP mit einem 5V Netzteil betreibt kann auch auf die Wiederstände und die Messung der Versorgungsspannung verzichten.

![Fritzing with voltage divider](/fritzing/zisterne01-voltage-divider.png)




## Aufbau und Installation

Der ESP samt Breakoutboad des Ultraschallsensor passt gut in eine etwas größere Aufputz-Abzweigdose. Die Gummimembranen von hochwertigen Abzweigdosen (3-5 €) erlauben es den Stecker des Ultaschallsensor und ggf. die Zuleitung der Versorgungsspannung wasserdicht in das Gehäuse einzuführen.
Ich habe die Dose an einer zuvor installierten Holzstrebe installiert und einen kleine Ausleger für den Fühler des Sensors angebaut. **Wichtig: Stellt sicher dass der Sensor "freie Sicht" auf die Wasseroberfläche hat. Rohre, Kabel o.ä. im Streubereich des Sensors (ca. 20 cm um den Sensor) können die Messung verfälschen.**

![Prototype board](/images/prototype.jpg)
![Case](/images/case.jpg)
![Installation](/images/installation2.jpg)

## Konfigurieren und kompilieren

Passt die Einträge wie:

* WiFi Netzwerkname und Passwort
* Hostname - unter diesem Namen könnt ihr über mdns im format HOSTNAME.local auf den Webserver zugrefen
* min_level, max_level untere bzw. obere Füllstandgrenze der Zisterne in cm 

im header der der ![main.cpp](/main/src/main.cpp) an und kompiliert den sketch mit arduino oder platformio. Nachdem ersten kompilieren wird der enstandene Binärcode via USB auf den ESP Microcontroller übertragen. Der Code ist OTA fähig, so dass zukünftige Updates - ohne USB-Kabel over-the-air übertragen werden können.

## Datacollection

Auf Port 9110 (konfigurierbar) werden die Messwerte des Sensors und weitere nützliche Metriken, wie die WiFi-Signalstärke, Höhe der Versorgungsspannung, uptime, usw. exportiert.
Das verwendete ![prrometheus exporter format](/images/screenshot-prometheus.png) kann leicht aufgeparst werden und in beliebige andere Automatisierungslösungen eingebunden werden. 

Wenn ihr bereits einen prometheus server am laufen habt könnt ihr eure *prometheus.yml* wie folgt erweitern und die Metriken langzeit archivieren. Der name des targets muss dem von euch in der main.cpp konfigurierten hostname entsprechen.

 `- job_name: 'Zisterne'
    #honor_labels: true
    static_configs:
      - targets: ['zisterne01.fritz.box:9110']`
      
     
  ## Visualisierung
  
  Landen die Daten erstmal in prometheus können mit Grafa schnell ansprechende Dashboards erstellt werden.
  
  ![Grafana dashbaord](/images/screenshot-grafana.png)




