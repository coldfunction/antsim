# antsim

## v0.1
* Ant
    * live 3 -> reproduce
    * move: random go

* Doodlebug
    * live 8 -> reproduce (birth like Ant)
    * move: eat prefer Ant
    * strvation: if no eat within 3 steps (HP = 3)

## v0.2

* Plant
    * HP to 10 -> reproduce (birth like Ant) 
    * HP-- if eaten by Ant
    * move: no
    * strvation: no

    

* Ant
    * live 3 -> reproduce
    * move: random go
    * strvation: if no eat within 5 steps (HP = 5), if eat, HP++ (MAX HP is 5)

* Doodlebug
    * live 8 -> reproduce (birth like Ant)
    * eat prefer Ant
    * strvation: if no eat within 3 steps (HP = 3)