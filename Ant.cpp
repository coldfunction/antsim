# include <iostream> 
#include <stdio.h>
#include <unistd.h>
#include <map>

#define _clear() printf("\033[H\033[J")
#define _disable_cursor() printf("\033[?25l")
#define _set_XY(x,y,s) printf("\033[%d;%dH%s", (y), (x), (s)); 
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

using namespace std;

class Matter {
    public: 
        Matter(int x, int y, string shape) : 
            pos_X(x+INIT_X), pos_Y(y+INIT_Y), shape(shape) {}
        Matter(int x, int y, string shape, bool inorganic) : 
            pos_X(x+INIT_X), pos_Y(y+INIT_Y), shape(shape), inorganic(inorganic) {}
        Matter(string shape) : shape(shape) {}
        Matter(string shape, bool inorganic) : shape(shape), inorganic(inorganic) {}
        Matter() = default;
        ~Matter() = default;

        int get_posX() {return pos_X;}
        int get_posY() {return pos_Y;}
        string get_shape() {return shape;}
        bool isInorganic() {return inorganic;}

        void set_posX(int x) {pos_X = x;}
        void set_posY(int y) {pos_Y = y;}
        void set_shape(string s) {shape = s;}

        virtual bool canEat(string eat) = 0;
        virtual bool eat(Matter **matter) = 0;
    
    private: 
        string shape;
        int pos_X;
        int pos_Y;
        bool inorganic = true;
};

class Empty : public Matter {
    public:
        using Matter::Matter;

};

class Organism : public Matter {
    public:
        Organism(int x, int y, string shape) : Matter(x, y, shape, false) {}
        Organism(string shape) : Matter(shape, false) {}

        bool canEat(string eat) {
            return this->Predation[eat];
        }
        bool eat(Matter **matter) {
            //if this can eat
            if(canEat((*matter)->get_shape())) { 
                this->set_posX((*matter)->get_posX());
                this->set_posY((*matter)->get_posY());
                delete *matter;
                *matter = this;
                return true;
            } else {
                //TODO: if equal, depend on somewhat..
                return false;
            }
        }
        int life;
        map<string, bool> Predation;
};

class Ant : public Organism {
    public:
        using Organism::Organism;
        Ant() : Organism(ANT_SYMBOL) {
            Predation[EMPTY_SYMBOL] = true;
        }
 };


class Border : public Matter {
    public:
        using Matter::Matter; 
        bool canEat(string eat) {return false;}
        bool eat(Matter **matter) {return false;}
};

class Space { 
    public: 

        Space(int width, int height) : width(width+2), height(height+2) {
            int total = this->width * this->height;
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
                if(matter[i]->isInorganic()) {
                    delete matter[i];
                }
            }
            delete [] matter;
            
        }

        void run() {
            _clear();
            _disable_cursor();
            for(int i = 0; i < width; i++) {
                for(int j = 0; j < height; j++) {
                    display(*matter[i*width+j]);
                }
            }

        }

        void display(Matter &matter) {
            _set_XY(matter.get_posX(), matter.get_posY(), matter.get_shape().c_str());
            fflush(stdout); 
        }

        //default is random
        template <typename MATTER>
        void gen_matter(MATTER matter, int number) {
            //int n = get_random_num(0, number);
            for(int i = 0; i < number; i++) {
                int pos = i+303;
                
                matter[i].eat(&(this->matter[pos]));
            }

        }

    private:
        int width;
        int height;
        Matter **matter;
}; 


int main() {
    Space space(300,300);
    Ant *ant = new Ant[300];
    space.gen_matter(ant, 1);


    //space.run();

    //cout << ant[1].get_shape() << endl;
    //cout << ant[100].get_shape() << endl;

    //if(ant) {
        cout << "delete ant" << endl;
        delete [] ant;
   // } 


    return 0;

}