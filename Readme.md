# ClawSearch

Ein Memory-Scanner-Plugin für x64dbg, inspiriert von Cheat Engine.

![](Screenshot.png)

## Über das Projekt

**ClawSearch** ist ein Plugin für den Debugger [x64dbg](https://x64dbg.com), das die bekannte Suche nach Speicherwerten im Stil von [Cheat Engine](https://www.cheatengine.org/) direkt in den Debugger integriert.

Das Plugin basiert auf dem Original-Projekt [codecat/ClawSearch](https://github.com/codecat/ClawSearch); in diesem Repository wird es als eigenständiger Fork weitergeführt.

## Funktionen

- **Erste Suche und Folgesuchen** („First Scan" / „Next Scan") über den gesamten Speicher des debuggten Prozesses
- **Unterstützte Wertetypen:**
  - Byte (1 Byte), `Int16` (2 Bytes), `Int32` (4 Bytes), `Int64` (8 Bytes)
  - `Float` (4 Bytes), `Double` (8 Bytes)
- **Suchkriterien beim ersten Scan:**
  - Genauer Wert (`=`)
  - Mehr als (`>`), Weniger als (`<`)
- **Suchkriterien bei Folgesuchen:**
  - Genauer Wert, Unverändert, Verändert
  - Größer, Kleiner, Erhöht, Verringert
- **Hexadezimale Eingabe** möglich
- Optional: **Pause des debuggten Prozesses während des Scannens**
- Feinjustierbare **Gleitkomma-Behandlung** (Abschneiden, Runden mit konfigurierbarer Genauigkeit)

## Installation

1. Lade die neueste Version von der [Releases-Seite](../../releases) herunter (oder baue das Plugin selbst, siehe unten).
2. Kopiere `ClawSearch.dp64` nach `x64dbg\x64\plugins\` und `ClawSearch.dp32` nach `x64dbg\x32\plugins\`.
3. Starte x64dbg neu. Das Plugin erscheint als eigener Eintrag im Plugin-Menü.

## Selbst bauen

### Voraussetzungen

- **Visual Studio 2019 oder neuer** (Community reicht) oder **Build Tools** mit der Komponente *Desktop-Entwicklung mit C++*
- Git (zum Klonen)
- MSVC-Toolset für **x64 und x86** (beide Plattformen werden gebaut)

### Projektstruktur

Das Repository enthält alle Abhängigkeiten direkt im Repo – ein Submodule-Init ist nicht nötig:

| Ordner | Inhalt |
|---|---|
| `ClawSearch/` | Der eigentliche Plugin-Quellcode (Scanner, UI, x64dbg-Anbindung) |
| `ClawSearch/pluginsdk/` | Das Plugin-SDK von x64dbg (Header + Import-Bibliotheken) |
| `iup/` | [IUP](https://www.tecgraf.puc-rio.br/iup/) GUI-Bibliothek – wird für das Plugin-Fenster benötigt |
| `cd/` | [CD](https://www.tecgraf.puc-rio.br/cd/) Canvas-Bibliothek – wird beim Linken indirekt über IUPs Canvas braucht (`iupcd`) |

> **Hinweis:** `cd/` wirkt auf den ersten Blick ungenutzt (der Plugin-Code ruft keine `cd*`-Funktionen auf), ist aber fürs Linken unverzichtbar, da `iupcd` die CD-Symbole auflöst.

### Bauen

1. Klone dieses Repository:
  ```sh
  git clone https://github.com/Orohal/ClawSearchOro.git
  ```
2. Öffne `ClawSearch.sln` in Visual Studio.
3. Baue die Lösung:
   - Konfiguration `Release`
   - Plattform `x64` (für 64-Bit-Prozesse) bzw. `x86` (für 32-Bit-Prozesse)
4. Die fertigen Plugins liegen unter `x64\Release\` bzw. `Release\`.

Alternativ per Kommandozeile mit MSBuild:

```sh
msbuild.exe ClawSearch.sln /m /verbosity:minimal /t:Rebuild /p:Configuration=Release /p:Platform=x64
msbuild.exe ClawSearch.sln /m /verbosity:minimal /t:Rebuild /p:Configuration=Release /p:Platform=x86
```

### CI/CD

Dieses Repository verwendet einen GitHub-Actions-Workflow (`.github/workflows/build.yml`), der bei jedem Push automatisch baut. Wird ein neuer Tag (z. B. `v1.0`) gepusht, erzeugt der Workflow automatisch ein Release mit fertigen `dp32`/`dp64`-Dateien.

## Lizenz

Copyright (C) 2019 der ursprüngliche Autor [codecat](https://github.com/codecat) sowie alle nachträglichen Autoren.

Dieses Projekt ist unter der **GNU General Public License v3.0** lizenziert. Details findest du in [`License.txt`](License.txt); die Lizenz ist auch bei der [FSF](https://www.gnu.org/licenses/gpl-3.0.html) verfügbar.

Da dieses Projekt ein Fork ist, bleibt die originale GPL-3.0-Lizenzdatei unverändert. Readme-Anpassungen und weitere Änderungen in diesem Fork stehen ebenfalls unter GPL-3.0.

## Credits

- **[codecat](https://github.com/codecat)** – Autor des ursprünglichen Projekts
- **[Orohal](https://github.com/Orohal)** – Pflege dieses Forks
- **[x64dbg](https://x64dbg.com)** und sein Plugin-SDK
- **[IUP](https://www.tecgraf.puc-rio.br/iup/)** und **[CD](https://www.tecgraf.puc-rio.br/cd/)** (Tecgraf, PUC-Rio) – GUI- und Canvas-Bibliotheken
- **[Cheat Engine](https://www.cheatengine.org/)** – Inspiration
