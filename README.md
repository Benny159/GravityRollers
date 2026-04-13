# Gravity Rollers - Murmel-Simulation 

Dieses Repository enthält das Unreal Engine 5 Projekt "Gravity Rollers", eine Murmel-Simulation, die im Rahmen der Projektarbeit für Fachinformatiker Anwendungsentwicklung an der Berufsschule Technik in Rostock entwickelt wird.

## Projektanfroderungen
* [Anforderungsdokument](https://github.com/Benny159/GravatyRollers/blob/main/Documentation/REQUIREMENTS.md)
* [IHK-Standards](https://www.ihk.de/rostock/aus-und-weiterbildung/pruefungen/abschlusspruefung/dokumentation-projektarbeit-2646884)

## 📝 Projektbeschreibung

Die Anwendung ist eine **zeitabhängige Simulation** einer Murmelbahn in einem Kinderzimmer-Setting. Der Benutzer kann die physikalischen Parameter für **mehrere Murmeln individuell konfigurieren**, um deren Rennen auf einer prozedural generierten Strecke zu simulieren und die Ergebnisse direkt miteinander zu vergleichen. Die Anwendung dient dazu, die Auswirkungen verschiedener Eingabeparameter und Zufallsfaktoren auf das Ergebnis eines physikalischen Systems visuell darzustellen und auszuwerten.

Das Projekt wird entwickelt, um alle funktionalen und nicht-funktionalen Anforderungen des Projektauftrags zu erfüllen.

## ✨ Kernfeatures

Das Projekt implementiert die folgenden, im Projektauftrag geforderten Features:

*  **Physik-Simulation:** Eine realistische, zeitabhängige Simulation, bei der die Zeit gemessen wird, die Murmeln für eine Strecke benötigen.
* **Mehrfach-Konfiguration:** Anwender können mehrere Murmeln für einen einzigen Simulationslauf anlegen und deren physikalische Eigenschaften individuell einstellen.
*  **Umfassende Eingabeparameter:** Für jede Murmel können **mindestens sieben physikalische Eigenschaften** über eine Benutzeroberfläche eingestellt werden, um deren Verhalten zu beeinflussen. Dazu gehören unter anderem:
    * **Größe (`Size`):** Verändert das Volumen und die Kollisionsabfrage der Murmel.
    * **Masse (`Weight` & `MaterialDensity`):** Beeinflusst die Trägheit und wie die Murmel auf Kräfte reagiert.
    * **Reibung (`Friction`):** Bestimmt, wie stark die Murmel vom Untergrund abgebremst wird.
    * **Elastizität (`Restitution`):** Definiert, wie stark die Murmel bei Kollisionen abprallt ("Bounciness").
    * **Winkeldämpfung (`AngularDamping`):** Steuert, wie schnell die Rotation der Murmel verlangsamt wird.
    * **Massenverteilung (`MassDistribution`):** Erlaubt eine exzentrische Masseverteilung, die zu einem "Eiern" der Murmel führt.
    * **Oberflächenrauheit (`SurfaceRoughness`):** Simuliert den Luftwiderstand bzw. die Dämpfung durch die Oberfläche.
*  **Zufallsereignisse:** Die Simulation integriert mindestens drei verschiedene Zufallsverteilungen, um unvorhersehbare Ereignisse zu modellieren:
    * **Gleichverteilung:** Für die prozedurale und faire Generierung der Rennstrecke aus einem Pool vordefinierter Segmente.
    * **Normalverteilung:** Zur Simulation von variablen, leichten Umwelteinflüssen wie "Windstößen", die kontinuierlich auf die Murmeln einwirken.
    * **Exponentialverteilung:** Steuert die unregelmäßigen Zeitabstände zwischen **seismischen Stößen** (z.B. ein Kind, das am Tisch wackelt). Diese globalen Ereignisse beeinflussen alle Murmeln gleichzeitig durch eine plötzliche Krafteinwirkung und werden durch einen Kamera-Shake visualisiert.
*  **Variable Simulationsgeschwindigkeit:** Die Abspielgeschwindigkeit der Simulation kann in mehreren Stufen (Pause, 0.5x, 1.0x, 2.0x, 4.0x) gesteuert werden, um die Anforderung von mindestens 3 Stufen zu erfüllen.
*  **Visuelle Auswertung:** Nach Abschluss eines Rennens werden die Ergebnisse (Zeit, Platzierung) aller Murmeln in einer grafischen Übersicht dargestellt und in Relation zu den gewählten Startparametern gesetzt.
*  **Intuitive GUI:** Eine grafische Benutzeroberfläche, die nach den Interaktionsprinzipien der **ISO 9241-110** gestaltet ist und einen klaren Anwendungsfluss (Menü → Konfiguration → Auswertung) bietet.

## 🛠️ Technisches Konzept & Architektur

Das Projekt wird mit einem Hybrid-Ansatz aus C++ und Blueprints umgesetzt, um Performance und Flexibilität zu kombinieren.

* **Kernlogik in C++:**
    * Die `AMarble`-Klasse dient als zentrale Logikeinheit für die Murmeln. Alle physikalischen Eigenschaften sind hier als `UPROPERTY` deklariert, um sie in Blueprints zugänglich zu machen. Die Berechnungen finden in C++ statt, um maximale Performance zu gewährleisten.
    * Physikalische Materialien werden zur Laufzeit dynamisch in der `CreatePhysicsMaterial()`-Methode erzeugt, um die Parameter (Reibung, Elastizität) direkt anwenden zu können.
    * Die Generierung der Zufallszahlen für die verschiedenen Verteilungen wird in einer C++ Helper-Klasse gekapselt.

* **Blueprints & UMG:**
    * Die gesamte Benutzeroberfläche (UI), inklusive Startmenü, Simulations-HUD und Auswertungsbildschirm, wird mit dem **Unreal Motion Graphics (UMG)** Framework in Blueprints erstellt.
    * Einzelne Streckensegmente, Hindernisse und visuelle Effekte werden als Actor-Blueprints implementiert, um ein modulares und leicht erweiterbares System zu schaffen.
    * Die Steuerung des Spielablaufs (z.B. das Starten der Simulation, das Wechseln der Geschwindigkeit) wird über einen Blueprint-basierten `GameMode` gesteuert.

* **Performance für Schulrechner:**
    *  Um die Lauffähigkeit auf den Zielsystemen zu garantieren, wird auf **statisches, gebackenes Lighting** gesetzt.
    * Assets sind bewusst **Low-Poly** gehalten.
    * Die Physikberechnungen werden durch einfache Kollisionskörper (Spheres, Capsules) optimiert.

## 🖱️ UI/UX-Konzept

 Die Benutzeroberfläche und das Nutzererlebnis stehen im Fokus und orientieren sich an der **ISO 9241-110**.

* **UI-Fluss:**
    1.  **Startmenü:** Einfacher Einstiegspunkt zum Starten der Simulation.
    2.  **Simulations-UI:** Hauptansicht mit einer Top-Down-Perspektive zur Konfiguration der Strecke und der Murmel-Parameter über klar beschriftete Schieberegler. Enthält Steuerungselemente für Start, Pause und Geschwindigkeit.
    3.  **Auswertungs-UI:** Separater Bildschirm nach dem Rennen zur Anzeige von Zeit und Platzierung, visualisiert durch Balkendiagramme.

* **Kameraführung:**
    * **Konfigurationsphase:** Eine statische Top-Down-Kamera für den Überblick.
    * **Simulationsphase:** Eine dynamische Verfolgerkamera, die sich optional auf eine der Murmeln fokussieren kann, oder eine Gesamtansicht der Strecke bietet.

* **Visuelle Effekte:**
    *  Ein **Partikel-Trail** an den Murmeln zur Visualisierung der Geschwindigkeit.
    *  Zusätzliche Effekte bei Kollisionen oder der Aktivierung von Streckenelementen (z.B. Boost-Pads), um dem Nutzer klares Feedback zu geben und die Anforderung einer passenden Animation zu erfüllen.

## 🚀 Tech Stack

* **Engine:** Unreal Engine 5
* **Sprache:** C++ / Blueprints (Hybrid-Ansatz)
* **Physik:** Unreal Chaos Physics
*  **Plattform:** Windows (64-bit)

## 📂 Projektmanagement

*  **Vorgehensmodell:** Das Projekt folgt dem **Wasserfallmodell**, da die Anforderungen von Beginn an klar und unveränderlich definiert sind.
*  **Abgabetermin:** 06.02.2025
*  **Dokumentation:** Die vollständige Projektdokumentation nach IHK-Standard und ein digitales Benutzerhandbuch werden separat erstellt.
