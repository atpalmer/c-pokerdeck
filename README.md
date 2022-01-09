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


## showdown

Runs heads-up simulations and computes showdown statistics.

Sample output:

```
            hand ( freq.):   wins (  win%); losses ( loss%);  chops ( chop%);  total
     Royal Flush ( 0.01%):     64 ( 97.0%);      0 (  0.0%);      2 (  3.0%);     66
  Straight Flush ( 0.04%):    358 ( 93.5%);      3 (  0.8%);     22 (  5.7%);    383
  Four of a Kind ( 0.33%):   3064 ( 91.7%);    179 (  5.4%);     98 (  2.9%);   3341
      Full House ( 5.08%):  44587 ( 87.8%);   2797 (  5.5%);   3386 (  6.7%);  50770
           Flush ( 6.19%):  52433 ( 84.7%);   7301 ( 11.8%);   2200 (  3.6%);  61934
        Straight ( 9.18%):  74061 ( 80.7%);   6496 (  7.1%);  11220 ( 12.2%);  91777
 Three of a Kind ( 9.69%):  70502 ( 72.7%);  22995 ( 23.7%);   3444 (  3.6%);  96941
        Two Pair (46.82%): 301351 ( 64.4%); 143166 ( 30.6%);  23652 (  5.1%); 468169
            Pair (87.81%): 358091 ( 40.8%); 491042 ( 55.9%);  28964 (  3.3%); 878097
       High card (34.85%):  54564 ( 15.7%); 285096 ( 81.8%);   8862 (  2.5%); 348522
```
