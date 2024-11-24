# *yafth* - Yet Another Fallout Terminal Hacking ~~Game~~
> Patrolling the Mojave almost makes you wish for a nuclear winter.

*yafth* is a C++ console application that replicates the terminal hacking experience from Fallout 3/4 and New Vegas.

## Demo

<p align="center">
  <img src="demo.gif" alt="animated" />
</p>

## Installation
```
git clone https://github.com/gshigin/yet-another-fallout-terminal-hacking-game.git yafth
cd yafth
mkdir build && cd build
cmake ..
cmake --build .
```

## Usage
```
Usage: ./yafth [options]
Options:
  -h, --help              Show this help message
  -s, --science VALUE     Science level (0-100)
  -l, --lock VALUE        Lock level (0-4 or very_easy, easy, average, hard, very_hard)
  --seed VALUE            Optional seed for random generator
```
All parameters are optional. In their absence, the seed is generated randomly, the Science skill is set to 50, and the terminal difficulty is set to [Average](https://fallout.fandom.com/wiki/Hacking). 

If the Science skill and the terminal difficulty are specified, the matching skill of the complexity is checked:
```
$ ./yafth -s 50 -l very_hard
Error: A Science skill of 100 is required to hack this terminal. >:(
```

If only a Science skill is specified, the hardest possible terminal difficulty is used (if any).

If only a terminal difficulty is specified, the minimum required Scince skill is used (always exists).

## Dependencies
This project uses the [FTXUI library](https://github.com/ArthurSonzogni/FTXUI) for building terminal-based user interfaces.

## License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.

This project includes code that depends on FTXUI, which is licensed under the MIT License.