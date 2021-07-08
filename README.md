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

