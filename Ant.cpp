#include <iostream>
#include <fstream> 
#include <random>
#include <algorithm>

#include <stdio.h>
#include <unistd.h>

#define _clear() printf("\033[H\033[J")
#define _disable_cursor() printf("\033[?25l")
#define _gotoXY(x,y) printf("\033[%d;%dH", (y), (x))
#define _set_XY(x,y,s) printf("\033[%d;%dH%s", (y), (x), (s)) 
#define INIT_X 2
#define INIT_Y 3

#define EMPTY_SYMBOL " "
#define BORDER_HORIZONTAL_SYMBOL "═"
#define BORDER_VERTICAL_SYMBOL "║"
#define BORDER_CORNER_LU_SYMBOL "╔"
#define BORDER_CORNER_RU_SYMBOL "╗"
#define BORDER_CORNER_LD_SYMBOL "╚"
#define BORDER_CORNER_RD_SYMBOL "╝"
#define ANT_SYMBOL "O"
#define DOODLEBUG_SYMBOL "X"

#define ANT_REPRODUCE_CYCLE 3
#define DOODLEBUG_REPRODUCE_CYCLE 8

class Space;

//int global_dc = 0;

using namespace std;

int get_random_num(int a, int b) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist6(a,b); // distribution in range [a, b]
    return dist6(rng);
}

enum Action {
    //STOP, RIGHT, LEFT, UP, DOWN
    RIGHT=1, LEFT, UP, DOWN
};

class Matter {
    public: 
        Matter(int x, int y, string shape) : 
            pos_X(x/*+INIT_X*/), pos_Y(y/*+INIT_Y*/), shape(shape) {}
        Matter(int x, int y, string shape, bool inorganic) : 
            pos_X(x/*+INIT_X*/), pos_Y(y/*+INIT_Y*/), shape(shape), inorganic(inorganic) {}
        Matter(string shape) : shape(shape) {}
        Matter(string shape, bool inorganic) : shape(shape), inorganic(inorganic) {}
        Matter() = default;
        ~Matter() = default;

        int get_posX() {return pos_X;}
        int get_posY() {return pos_Y;}
        string get_shape() {return shape;}
        bool isInorganic() {return inorganic;}
        virtual bool isActived() {return moved;}
        virtual void actived() {moved = true;}
        virtual void reset_actived() {moved = false;}

        void set_posX(int x) {pos_X = x;}
        void set_posY(int y) {pos_Y = y;}
        void set_shape(string s) {shape = s;}

        virtual bool canEat(string eat) {return false;}
        virtual bool eat(Matter **matter) {return false;}
        virtual int move(Space *matter) {return -1;}
        virtual void reproduce() {}
    
    private: 
        string shape;
        int pos_X;
        int pos_Y;
        bool inorganic = true;
        bool moved = false;
};
/*
class Empty : public Matter {
    public:
        using Matter::Matter;

};*/

class Border : public Matter {
    public:
        using Matter::Matter; 
};

class Organism : public Matter {
    public:
        Organism(int x, int y, string shape) : Matter(x, y, shape, false) {
            Predation.push_back(EMPTY_SYMBOL);
        }
        Organism(string shape) : Matter(shape, false) {
            //Predation[EMPTY_SYMBOL] = true;
            Predation.push_back(EMPTY_SYMBOL);
        }

        bool canEat(string eat) {
            return (find(Predation.begin(), Predation.end(), eat) != Predation.end());
        }
        bool eat(Matter **matter) {
            //if this can eat
            //if(this->get_shape()=="X" && (*matter)->get_shape() == "O") {
                //cout << "ready to eat " << endl;
                //cout << "source = " << this->get_posX() << " : " << this->get_posY() << endl;
                //cout << "des = " << (*matter)->get_posX() << " : " << (*matter)->get_posY() << endl;
            //}
            if(canEat((*matter)->get_shape())) { 
                this->set_posX((*matter)->get_posX());
                this->set_posY((*matter)->get_posY());
                delete *matter;
                *matter = this;
                
                //cout << "after eat source = " << this->get_posX() << " : " << this->get_posY() << endl;

             //   cout << "after eat" << endl;
                return true;
            } else {
                //TODO: if equal, depend on somewhat..
                return false;
            }
        }
        
    protected:
        int reproduce_cycle;
        vector<string> Predation;
        int life_cycle = 0;
};

class Space { 
    public: 

        Space(int width, int height) : width(width+2), height(height+2) {
            int total = this->width * this->height;
            cout << "cocotion test total =" << total << endl;
            matter = new Matter*[total];
// ╔════X═════╗
// Y          ║
// ╚══════════╝
            for(int i = 0; i < this->height; i++) {
                for(int j = 0; j < this->width; j++) {
                    if(i == 0 || i == this->height-1) {
                        if(j > 0 && j < this->width-1) {
                            matter[i*(this->width)+j] = new Border(j, i, BORDER_HORIZONTAL_SYMBOL);
                        } else {
                            if(i == 0 && j == 0) { 
                                matter[i*(this->width)+j] = new Border(j, i, BORDER_CORNER_LU_SYMBOL);
                            } else if (i == 0 && j == this->width-1) {
                                matter[i*(this->width)+j] = new Border(j, i, BORDER_CORNER_RU_SYMBOL);
                            } else if (i == this->height-1 && j == 0) {
                                matter[i*(this->width)+j] = new Border(j, i, BORDER_CORNER_LD_SYMBOL);
                            } else if (i == this->height-1 && j == this->width-1) {
                                matter[i*(this->width)+j] = new Border(j, i, BORDER_CORNER_RD_SYMBOL);
                            }
                        }
                    } else {
                        //EMPTY or VER
                        if((j == 0 || j == this->width-1) && (i > 0 && i < this->height-1 )) {
                            matter[i*(this->width)+j] = new Border(j, i, BORDER_VERTICAL_SYMBOL);
                        } else {
                            matter[i*(this->width)+j] = new Border(j, i, EMPTY_SYMBOL);
                        }
                    }
                }
            }
        }
        
        ~Space() { 
            
            int total = width * height;
            for(int i = 0; i < total; i++) {
                //if(matter[i]->isInorganic()) {
                    delete matter[i];
               // }
            }
            delete [] matter;
            
        }

        void initMap() {
            _clear();
            _disable_cursor();
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    display(*matter[i*width+j]);
                }
            }
            _gotoXY(width+3, height+4);


        }

        int updatePos(Matter *matter, int action) {
            int X = matter->get_posX();
            int Y = matter->get_posY();
            //cout << "action = " << action << endl;
            //cout << "before X, Y = " << X <<"," << Y << endl;
            switch(action) {
                case Action::RIGHT:
                    ++X; 
                    break;
                case Action::LEFT:
                    --X;
                    break;
                case Action::UP:
                    ++Y;
                    break;
                default:    
                    --Y;
            }
            //cout << "after X, Y = " << X <<"," << Y << endl;
            //cout << "update pos = " << Y*width+X << endl; 
            return Y*width+X;
        }

        bool try_jump(int src, int des) {
            if (this->matter[src]->eat(&(this->matter[des]))) {
                int y = src/width;
                int x = src%width;
                this->matter[src] = new Border(x, y, EMPTY_SYMBOL);
                display(*(this->matter[src]));
                display(*(this->matter[des]));
                return true;
            }
            return false;
        }


        void organism_move() {
            int num = width*height;
            for(int i = 0; i < num; i++) { //if not ig
                if(!(this->matter[i]->isInorganic()) && !(this->matter[i]->isActived())) {
                    int newPos = this->matter[i]->move(this);
                    //cout << "old, new = " << i << " : " << newPos << endl;
                    //cout << "shape = " << this->matter[i]->get_shape() << endl;
                    if(try_jump(i, newPos)) {
                        this->matter[newPos]->actived();
                        //try_reproduce(i);
                    }
                }
            }
            for(int i = 0; i < num; i++) { //if not ig
                if(!(this->matter[i]->isInorganic()) && (this->matter[i]->isActived())) {
                    this->matter[i]->reset_actived();
                }
            }
        }

        void run() {
            initMap();
            for(int i = 0; i < 1000; i++) {
                organism_move();
                sleep(1);
            }
        }

        void display(Matter &matter) {
           /* if(matter.get_shape() == "O") {
                global_dc++;
                //cout << matter.get_shape() << " X: "<< matter.get_posX() << "Y: "<< matter.get_posY() << " count:"<< global_dc << endl;
                _set_XY(matter.get_posX(), matter.get_posY(), matter.get_shape().c_str());
                fflush(stdout); 
            }*/
            _set_XY(matter.get_posX()+INIT_X, matter.get_posY()+INIT_Y, matter.get_shape().c_str());
            fflush(stdout); 
        }

        //default is random
        template <typename MATTER>
        void gen_matter(MATTER matter, int number) {
            //ofstream out("info.txt");

            for(int i = 0; i < number; i++) {
                //int pos = i+303;
                //if eat sucessfully return true 
                int pos;
  //              do {
 //                   pos = get_random_num(0, width*height-1);
//                } while(!matter[i].eat(&(this->matter[pos])));
                do {
                    pos = get_random_num(0, width*height-1);
                } while(this->matter[pos]->get_shape() != EMPTY_SYMBOL);
                //TODO: update x, y  by function
                int x = this->matter[pos]->get_posX();
                int y = this->matter[pos]->get_posY();
                delete this->matter[pos];
                this->matter[pos] = new MATTER(x,y);

                //out << this->matter[pos]->get_shape() << "X: "<< this->matter[pos]->get_posX() << "Y: "<< this->matter[pos]->get_posY() << endl;
                //out << this->matter[pos]->isInorganic();
            }
            //out.close();
        }

    private:
        int width;
        int height;
        Matter **matter;
}; 

class Ant : public Organism {
    public:
        using Organism::Organism;
        Ant() : 
        Organism(ANT_SYMBOL) {
            reproduce_cycle = ANT_REPRODUCE_CYCLE;
        }
        Ant(int x, int y) : 
        Organism(x, y, ANT_SYMBOL) {
            reproduce_cycle = ANT_REPRODUCE_CYCLE;
        }
        int move(Space *space) {
            int action = get_random_num(Action::RIGHT, Action::DOWN);
            return space->updatePos(this, action); 
        }
        void reproduce() {


        }
 };

class Doodlebug : public Organism {
    public:
        using Organism::Organism;
        Doodlebug() : 
        Organism(DOODLEBUG_SYMBOL) {
            Predation.push_back(ANT_SYMBOL);
            reproduce_cycle = DOODLEBUG_REPRODUCE_CYCLE;
        }
        Doodlebug(int x, int y) : 
        Organism(x, y, DOODLEBUG_SYMBOL) {
            Predation.push_back(ANT_SYMBOL);
            reproduce_cycle = DOODLEBUG_REPRODUCE_CYCLE;
        }
        int move(Space *space) {
            int action = get_random_num(Action::RIGHT, Action::DOWN);
            return space->updatePos(this, action); 
        }
        void reproduce() {}
};



int main() {
    //Space space(300,300);
    //Space space(120,20); //ok
    Space space(20,20); //ok
    //Doodlebug *doodlebug = new Doodlebug[300];
    Doodlebug doodlebug;

    //Ant *ant = new Ant[300];
    Ant ant;
    //space.gen_matter(ant, 300);

    space.gen_matter(doodlebug, 10);
    space.gen_matter(ant, 10);


    space.run();


    cout << "delete ant" << endl;
   // delete [] ant;
//    delete [] doodlebug;


    return 0;

}