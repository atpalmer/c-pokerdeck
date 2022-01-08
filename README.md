# pokerdeck

This project includes sevaral simple utilities related to poker hands.

* The `src` directory contains sources related to modeling a poker game and evaluating hands.
* The `prog` directory contains sources for specific console utilities (output to `bin`).
* The `include` directory contains headers needed for compilation.


## playround

Just a simple utility to deal random poker boards.

Sample output:

```
Hero:
	2s Qc
Villain:
	7s 2c
Flop:
	9d 6c 3h
Turn:
	9d 6c 3h 3s
River:
	9d 6c 3h 3s Qs
Result:
      Hero: Two Pair [QQ339]
   Villain: Pair [33Q97]
    Winner: Hero
```


## tophands

Runs heads-up simulations and computes the top starting hands based on win percentages.

Sample output:

```
Top starting hands for 1000000 simulations:
1) [AAo]: 84.6%
2) [KKo]: 82.3%
3) [QQo]: 79.5%
4) [JJo]: 76.8%
5) [TTo]: 73.2%
6) [99o]: 72.1%
7) [AKs]: 68.1%
8) [88o]: 67.4%
9) [AQs]: 65.5%
10) [AKo]: 64.9%
11) [ATs]: 64.9%
12) [77o]: 64.9%
13) [AJs]: 64.2%
14) [AQo]: 64.0%
15) [AJo]: 63.2%
16) [KQs]: 63.0%
17) [66o]: 62.5%
18) [KJs]: 62.3%
19) [A9s]: 62.0%
20) [ATo]: 61.8%
...
150) [74o]: 34.9%
151) [62s]: 34.9%
152) [72s]: 34.6%
153) [83o]: 34.6%
154) [63s]: 34.5%
155) [54o]: 34.2%
156) [52s]: 33.9%
157) [42s]: 33.7%
158) [64o]: 33.6%
159) [82o]: 33.6%
160) [73o]: 33.1%
161) [32s]: 33.0%
162) [53o]: 31.9%
163) [63o]: 31.7%
164) [43o]: 31.3%
165) [72o]: 31.3%
166) [62o]: 29.7%
167) [52o]: 29.7%
168) [42o]: 29.6%
169) [32o]: 28.0%
```

