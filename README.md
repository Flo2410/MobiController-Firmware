# MobiController Firmware

Dies ist die Firmware für den Mobi des Robotikstudiengangs der FHWN.

## Voraussetzungen

Zur Konfiguration des Projekts wird die Software [STM32 CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) benötigt. </br>
Als Entwicklungsumgebung wird VS Code mit der Erweiterung [stm32-for-vscode](https://marketplace.visualstudio.com/items?itemName=bmd.stm32-for-vscode) verwendet.

## Flashen von einem Release

Um einen vorhandenen Release zu flashen, muss zuerst die entsprechende `MobiController-Firmware.elf` von der [Releaseseite](https://github.com/Flo2410/MobiController-Firmware/releases) heruntergeladen werden.
Anschließend muss die ELF Datei in den Build-Ornder verschoben werden, sodass der Dateipfad `./build/MobiController-Firmware.elf` lautet.
Der Flash-Prozess kann nun mittels folgendem Befehl gestartet werden.

```bash
make -j16 -f STM32Make.make flash
```

## Kompilieren

Um das Projekt zu kompilieren, muss lediglich die _Build_ Funktion der STM32 Erweiterung ausgeführt werden.
Dies kann entweder über das Menü der Erweiterung oder über den Befehl `STM32: Build STM32 project` mittels der Befehlspalette gemacht werden.

## Kompilieren und Flashen

Um die Firmware auf den STM32 zu flashen, wird die Funktion _Flash STM32_ der Erweiterung verwendet.
Diese ist ebenfalls über das Menü, sowie über die Befehlspalette als `STM32: Build and flash to an STM32 platform` erreichbar.
