# How to design a blockchain-based provably fair online dice game

In the old times, when we play a traditional dice game, the game fairness is usually endorsed by the game owner's reputation. If an extremely unlucky player loses 10 times in a row, there is no way for him to tell if he is just being unlucky or not.

With the help of the blockchain technology, now we have a way to design an online provably fair dice game technically. Blockchain is a transparent, tamper-resistant online ledger that every one can audit.

We are glad to see dozens of online dice games popping up in the EOS community recently. It's a pity that most of them are not open sourced nor rig resistant, still leaving inapparent potential backdoors for rigged betting, which is risky and can be largely improved. However, these close sourced dice games did help many rookies embrace the blockchain and get to know the beauty of the EOS world. Their efforts should not be forgotten and we appreciate their value added to the EOS ecology.

Hereby our DappPub team would like to share the design for our open sourced FairDice game and enlighten those who are interested to unleash the power of DApps together.



## 1. Randomness is the key

The key point is that the game has to provide a kind of combined randomness that no single party could control alone.

| ["How can I generate random numbers inside a smart contract?"](https://eosio.stackexchange.com/questions/41/how-can-i-generate-random-numbers-inside-a-smart-contract) |
| :----------------------------------------------------------: |
| ![image-20180913144954664](https://raw.githubusercontent.com/Dappub/fairdicegame/master/img/bm.png) |

Just like what BM answered in the above question on StackExchange, for a two-party (```player```-```house```) dice game, the key point to provide fairness or rig-resistant randomness requires a 3 step process:

> 1. Both parties  (  ```player``` & ```house```  ) commit hashes of their seeds:
>
>    ```hash(player_seed)```  &   ```hash(house_seed)```
>
> 2. Both parties reveal their ```hash(player_seed)```  &  ```hash(house_seed)```
>
> 3. ```Roll``` = ```hash(player_seed + house_seed)```

The final ```Roll``` is random and it's a kind of combined randomness based on both ```house``` &&  ```player``` 's input (```house_seed```  &  ```player_seed```)



## 2. Sequence Diagram: a theoretical random dice roll process

Based on BM's answer, we can draw a simple sequence diagram for a theoretical
random dice roll process as below:

|   Sequence Diagram: a theoretical random dice roll process   |
| :----------------------------------------------------------: |
| ![image-20180915124605622](https://raw.githubusercontent.com/Dappub/fairdicegame/master/img/theoretical.png) |

In step 2, the ```house``` creates a new ```house_seed``` for each bet and keeps it a secret until step 10.

In step 3, the ```house``` calculates the corresponding ```house_hash``` =  ```hash(house_seed)```.

In step 4, the ```house``` uploads the  ```house_hash``` to the blockchain. At this moment, the ```player``` still has no idea about what the ```house_seed``` is, but he is sure that the ```house_seed``` will not change during this bet because he can use the  ```house_hash``` to verify after the bet. Metaphorically speaking, ```house_hash``` is like a ```house```'s dark card placed on the table.

In step 5, the ```player``` also creates his ```player_seed```.

In step 6, the ```player``` calculates the corresponding ```player_hash``` = ```hash(player_seed)```.

In step 7, the ```player``` uploads ```player_hash``` to the blockchain. At this moment, the ```house``` still has no clue about what ```player_seed``` is, but he is sure that the ```player_seed``` will not change during this bet because he can use the ```player_hash``` to verify later. Metaphorically speaking, ```player_hash``` is like a ```player```'s dark card placed on the table.

In step 8, the ```player``` uploads his ```player_seed``` to the blockchain.

In step 9, the open-sourced smart contract deployed on the blockchain verifies the ```player_seed``` & ```player_hash``` to see if it's a match.

In step 10, the ```house``` uploads his ```house_seed``` to the blockchain.

In step 11, the smart contract verifies the ```house_seed``` & ```house_hash``` to see if it's a match.

In step 12, the smart contract calculates the ```hash(player_seed, house_seed)``` as the final roll number.



## 3. Sequence Diagram: optimized "FairDice" random dice roll process

Now we can give it a shot to start building our own blockchain-powered dice game. Compared with other blockchain infrastructure, EOS is preferred here as it's fast and provides good user experience.

In fact, to optimize the whole process, it's unnecessary for ```player``` to upload both ```player_hash``` & ```player_seed``` in two steps.

In other words, since the ```house``` has already placed his dark card (```house_hash```) on the table and not able to change it during the bet, the ```player``` could just place his open card (```player_seed```) on the table directly, instead of placing his dark card (```player_hash```) on the table and flipping the card (```player_seed```) in two steps.

Therefore, we have optimized the whole process and improved user experience as below:

| Sequence Diagram: optimized "FairDice" random dice roll process |
| :----------------------------------------------------------: |
| ![image-20180915125933975](https://raw.githubusercontent.com/Dappub/fairdicegame/master/img/optimized.png) |

In step 1, since a "referral bonus" is introduced in the game, the ```referrer``` along with the```ROLL under to win``` will be sent to the ```house```.

In step 2, the ```house``` creates a new ```house_seed``` for each bet and keeps it a secret until step 9.

In step 3, the ```house``` calculates the corresponding ```house_hash``` =  ```hash(house_seed)```.

In step 4, the ```house``` creates ```house_sign``` = ```sign(ROLL_under, hash(house_seed), expiration_time, referrer)```.

In step 5, the ```house``` sends ```house_hash```, ```house_sign```, ... to the ```player```.

In step 6, the ```player``` creates his ```player_seed```.

In step 7, the ```player``` uploads the ```house_sign``` & ```player_seed``` to the EOS blockchain.

In step 8, the smart contract deployed on EOS checks to prevent ```player``` from replay attacking the previous ```house_seed``` by cheating in the ```house_sign```.

In step 9, the ```house``` uploads the ```house_seed```.

In step 10, the smart contract verifies the ```house_seed``` to see if it matches with the ```house_hash``` so as to prevent ```house``` from swapping the ```house_seed``` sneakingly.

In step 11, the smart contract calculates the final ```roll``` num based on the ```player_seed``` & ```house_seed```.



## 4. Talk is cheap, show me the code

In this case, since the randomness of the final roll number is dependent on both ```house``` side and ```player``` side, neither the ```house``` nor the ```player``` is able to rig the bet alone. In order to be rig resistant and provably fair, undoubtedly, the dice game has to be open sourced:

> https://github.com/Dappub/fairdicegame


You could find more info here:

> https://medium.com/dapppub/fairdicegame-187afaff9e13



------



| ![logo@3x](https://raw.githubusercontent.com/Dappub/fairdicegame/master/img/logo.png) |
| -----------------------------------------------------------: |
| [**DappPub: Unleashing the power of DApps**](https://dapp.pub) |
