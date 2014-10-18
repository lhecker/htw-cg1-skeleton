## Übersicht

Dies ist ein Skeleton Projekt für die Erstellung von Cross-Platform Projekten im CG1 Modul der HTW Dresden.

## Abhängigkeiten

* C++11 Compiler (u.a. Xcode 6, Visual Studio 2013, gcc, …)
* Python 2.6 oder neuer
* gyp (wird automatisch beim Aufruf von configure installiert).

## Build Anweisungen

### OS X / Linux

    $ ./configure -f [cmake|eclipse|make|msvs|ninja|xcode]

Also z.B. für Xcode:

    $ ./configure -f xcode

## Verwendung

Dieses Projekt enthält eine rudimentäre main() Funktion welche ein Fenster kreiert und die Funktionen

- void init();
- void display();

in einer anderen Translation Unit (sprich einer anderen C/C++ Datei) erwartet.
Diese werden dann zum einmaligen initialisieren der Szene und danach (offensichtlich) als Display-Callback verwendet.

## Unterstützte Plattformen

__Zur Zeit wurden nur OS X 10.10 und Xcode 6.1 getestet.__

Windows und Linux, sowie andere Build Systeme müssen noch getestet werden,
sollten jedoch theoretisch mit höchstens kleinen Änderungen sofort funktionieren.
