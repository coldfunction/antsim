#include <iostream>
#include <fstream> 
#include <random>
#include <algorithm>
#include <map>
#include <boost/process.hpp>

#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>

#include "cxxopts.hpp"

#define _clear() printf("\033[H\033[J")
#define _disable_cursor() printf("\033[?25l")
#define _enable_cursor() printf("\e[?25h")
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
#define UNIVERSAL_SYMBOL "氣"
#define ANT_SYMBOL "O"
#define DOODLEBUG_SYMBOL "X"
#define PLANT_SYMBOL "▓" 

#define SPECIES_NUM 3

#define ANT_REPRODUCE_CYCLE 7
//#define DOODLEBUG_REPRODUCE_CYCLE 8
#define DOODLEBUG_REPRODUCE_CYCLE 212
#define PLANT_REPRODUCE_CYCLE 28

//#define DOODLEBUG_HP 3
#define DOODLEBUG_HP 210
#define ANT_HP 7
#define PLANT_HP 2

class Space;

using namespace std;

class Info {
    public: 
        Info() {
            boost::process::system("rm info.txt"); 
            out.open("info.txt", ios::app);
            species[ANT_SYMBOL] = 0;
            species[DOODLEBUG_SYMBOL] = 0;
            species[PLANT_SYMBOL] = 0;
            //boost::process::spawn("python3 info.py");
        }
        ~Info() {
            out.close(); 
        }
        void count_num_show() {
            boost::process::spawn("python3 info.py");
        }
        void show() {
            cycle++;
            out << cycle << " "<< species[ANT_SYMBOL] << " " << species[DOODLEBUG_SYMBOL] 
            <<" "<< species[PLANT_SYMBOL] << endl;
        } 
        void dec(string shape) {
            if (species.find(shape) != species.end())
                species[shape]--;
        }
        void add(string shape) {
            if (species.find(shape) != species.end())
                species[shape]++;
        }
    private:
        ofstream out;
        map<string, int> species;
        int cycle = 0;
};

Info info;


int get_random_num(int a, int b) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(a,b); // distribution in range [a, b]
    return dist(rng);
}

enum Action {
    SAME=0, RIGHT=1, LEFT, UP, DOWN
};

class Matter {
    public: 
        Matter(int x, int y, string shape) : 
            pos_X(x), pos_Y(y), shape(shape) {}
        Matter(int x, int y, string shape, bool inorganic) : 
            pos_X(x), pos_Y(y), shape(shape), inorganic(inorganic) {}
        Matter(string shape) : shape(shape) {}
        Matter(string shape, bool inorganic) : shape(shape), inorganic(inorganic) {}
        Matter() = default;
        ~Matter() = default;

        int get_posX() {return pos_X;}
        int get_posY() {return pos_Y;}
        string get_shape() {return shape;}
        bool isInorganic() {return inorganic;}
        virtual bool isActived() {return moved;}
        virtual int actived() {moved = true; return -1;}
        virtual void reset_actived() {moved = false;}

        void set_posX(int x) {pos_X = x;}
        void set_posY(int y) {pos_Y = y;}
        void set_shape(string s) {shape = s;}
        
        void move_done() {moved = true;}

        virtual bool canEat(string eat) {return false;}
        virtual bool eat(Matter **matter) {return false;}
        virtual int move(Space *matter) {return -1;}
        virtual Matter* childbirth(int x, int y) {return nullptr;}
        virtual void reproduce(Space *space) {}
        virtual void rebirth(Space *space, string wantGo) {}
        virtual int get_newPos(Space *space, int oldPos, string wantGo) {return -1;}
        virtual int go_where(Space *space, int oldPos) {return -1;}
        virtual bool strvation() {return false;}
        virtual void refillHP() {return;}
        virtual void dec_HP() {return;}
        virtual int get_HP() {return 0;}

    private: 
        string shape;
        int pos_X;
        int pos_Y;
        bool inorganic = true;
    protected:
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
                    delete matter[i];
            }
            delete [] matter;
            _enable_cursor();
        }
        void add_species_pos(string shape, int pos) {
            species_pos[shape].push_back(pos);
        }
        void reset_species_pos(string shape) {
            species_pos[shape].clear();
        }
        void initMap() {
            _clear();
            _disable_cursor();
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    display(*matter[i*width+j]);
                }
            }
            //_gotoXY(width+3, height+4);
            fflush(stdout); 

        }
        int updatePos(Matter *matter, int action) {
            int X = matter->get_posX();
            int Y = matter->get_posY();
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
            return Y*width+X;
        }

        bool try_jump(int src, int des) {
            if(des < 0) return false;
            matter[src]->dec_HP();

            bool des_isInorganic = matter[des]->isInorganic(); 
            if (matter[src]->eat(&matter[des])) {
                //if(!des_isInorganic) {
                //    matter[des]->refillHP();
                //}
                int y = src/width;
                int x = src%width;
                matter[src] = new Border(x, y, EMPTY_SYMBOL);
                display(*matter[src]);
                display(*matter[des]);
                return true;
            }
            return false;
        }

        void try_reproduce(int pos) {
            matter[pos]->reproduce(this);
        }
        inline void run(int i, string shape) {
            if((matter[i]->get_shape() == shape) && !(matter[i]->isActived())) {
                // Doodlebug will try to prey. If fail to prey, HP--;
                int newPos = matter[i]->go_where(this, i);
                int update;
                // try_jump: deal how to eat
                if(try_jump(i, newPos)) {
                    update = newPos;
                } else {
                    update = i;
                }
                int born_cycle = matter[update]->actived();

                // Born first 
                if(born_cycle == 0) {
                    try_reproduce(update);
                }
                // Die after born
                if(matter[update]->strvation()) {
                    info.dec(matter[update]->get_shape());
                    int x = matter[update]->get_posX();
                    int y = matter[update]->get_posY();
                    delete matter[update];
                    matter[update] = new Border(x, y, EMPTY_SYMBOL);
                    display(*matter[update]);
                }
            } else if ((runOrder[0] == shape) && !(matter[i]->isInorganic()) && !(matter[i]->isActived())) {
                add_species_pos(matter[i]->get_shape(), i);
            }
        }
        void organism_move() {
            int num = width*height;
            for(int order = 0; order < SPECIES_NUM; order++) {
                string shape = runOrder[order];
                if(order == 0) {
                    for(int i = 0; i < num; i++) { //if not ig
                        run(i,shape);
                    }
                } else {
                    for(int i : species_pos[shape]) {
                        run(i,shape);
                    }
                }
            }
            for(int i = 0; i < num; i++) { //if not ig
                if(!(matter[i]->isInorganic()) && matter[i]->isActived()) {
                    matter[i]->reset_actived();
                }
            }
            for(int order = 1; order < SPECIES_NUM; order++) {
                string shape = runOrder[order];
                species_pos[shape].clear();
            }

            fflush(stdout); 
        }

        void run() {
            initMap();
            for(int i = 0; i < 1000000; i++) {
                //fgetc(stdin);
                organism_move();
                info.show();
                //sleep(1);
            }
        }

        void display(Matter &matter) {
            _set_XY(matter.get_posX()+INIT_X, matter.get_posY()+INIT_Y, matter.get_shape().c_str());
        }

        //default is random
        void gen_matter(Matter *matter, int number) {
            //ofstream out("info.txt");
            for(int i = 0; i < number; i++) {
                //if eat sucessfully return true 
                int pos;
                do {
                    pos = get_random_num(0, width*height-1);
                } while(this->matter[pos]->get_shape() != EMPTY_SYMBOL);
                //TODO: update x, y  by function
                int x = this->matter[pos]->get_posX();
                int y = this->matter[pos]->get_posY();
                delete this->matter[pos];
                this->matter[pos] = matter->childbirth(x,y);
                //out << this->matter[pos]->get_shape() << "X: "<< this->matter[pos]->get_posX() << "Y: "<< this->matter[pos]->get_posY() << endl;
                //out << this->matter[pos]->isInorganic();
            }
            //out.close();
        }
        Matter **get_matter() {return matter;}
        int get_width() {return width;}
    private:
        int width;
        int height;
        Matter **matter;
        string runOrder[SPECIES_NUM] = {
            //DOODLEBUG first
            DOODLEBUG_SYMBOL, 
            ANT_SYMBOL,
            PLANT_SYMBOL
        };
        map<string, vector<int>> species_pos;
}; 


class Organism : public Matter {
    public:
        Organism(int x, int y, string shape) : Matter(x, y, shape, false) {
            Predation.push_back(EMPTY_SYMBOL);
        }
        Organism(string shape) : Matter(shape, false) {
            Predation.push_back(EMPTY_SYMBOL);
        }

        bool canEat(string eat) {
            return (find(Predation.begin(), Predation.end(), eat) != Predation.end());
        }
        bool eat(Matter **matter) {
            //if this can eat
            if(canEat((*matter)->get_shape())) { 
                if(!((*matter)->isInorganic())) {
                    this->refillHP();
                    (*matter)->dec_HP();
                    if((*matter)->get_HP() != 0) {
                        return false;
                    }
                }
                info.dec((*matter)->get_shape());
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
        int get_newPos(Space *space, int oldPos, string wantGo) {
            if(wantGo == UNIVERSAL_SYMBOL) {
                return move(space);
            }
            int det[4] = {-1}; //U, D, L, R
            int newPos;
            do {
                newPos = move(space);
                int dpos = oldPos - newPos;
                if(det[0]+det[1]+det[2]+det[3] == 0) {
                    return -1;
                    //break;
                } else {
                    if(dpos == 1) det[0] = 0;
                    else if(dpos == -1) det[1] = 0;
                    else if(dpos > 1) det[2] = 0;
                    else det[3] = 0;
                }
            } while ((space->get_matter())[newPos]->get_shape() != wantGo);
            return newPos;
        } 
        void rebirth(Space *space, string wantGo) {
            int width = space->get_width();
            int x = this->get_posX();
            int y = this->get_posY();
            int oldPos = y*width+x;
            int newPos = get_newPos(space, oldPos, wantGo);
            if(newPos < 0) return;

            //newPos is ok, ready to born
            //TODO: take get_matter by temp pointer
            x = (space->get_matter())[newPos]->get_posX();
            y = (space->get_matter())[newPos]->get_posY();
            delete (space->get_matter())[newPos]; //delete EMPTY_SYMBOL
            (space->get_matter())[newPos] = childbirth(x, y);
            (space->get_matter())[newPos]->move_done();
            space->display(*((space->get_matter())[newPos]));
        
        }
        void reproduce(Space *space) {
            rebirth(space, EMPTY_SYMBOL);
        }
        int actived() {
            moved = true;
            born_cycle = (born_cycle+1)%reproduce_cycle;
            return born_cycle;
        }
        int move(Space *space) {
            int action = get_random_num(Action::RIGHT, Action::DOWN);
            return space->updatePos(this, action); 
        }
    protected:
        int reproduce_cycle;
        vector<string> Predation;
        int born_cycle = 0;
        //int HP = DOODLEBUG_HP;
};

class Ant : public Organism {
    public:
        using Organism::Organism;
        Ant() : 
        Organism(ANT_SYMBOL) {
            Predation.push_back(PLANT_SYMBOL);
            reproduce_cycle = ANT_REPRODUCE_CYCLE;
        }
        Ant(int x, int y) : 
        Organism(x, y, ANT_SYMBOL) {
            Predation.push_back(PLANT_SYMBOL);
            reproduce_cycle = ANT_REPRODUCE_CYCLE;
        }
        Matter* childbirth(int x, int y) {
            info.add(ANT_SYMBOL);
            return new Ant(x,y);
        }
        int go_where(Space *space, int oldPos) {
            return get_newPos(space, oldPos, UNIVERSAL_SYMBOL);
        }
        void refillHP() {
            if(HP < ANT_HP) {
                HP+=2; 
            }
        }
        void dec_HP() {HP--;}
        void inc_HP() {HP++;}
        int get_HP() {return HP;}
        bool strvation() {
            return !HP;
        }
    private:
        int HP = ANT_HP;
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
        Matter* childbirth(int x, int y) {
            info.add(DOODLEBUG_SYMBOL);
            return new Doodlebug(x,y);
        }
        int go_where(Space *space, int oldPos) {
            int pos = get_newPos(space, oldPos, ANT_SYMBOL);
            if(pos == -1) {
                //fail to eat
               // --HP;
               return get_newPos(space, oldPos, UNIVERSAL_SYMBOL);
            } else {
                //refillHP();
                return pos;
            }
        }
        void dec_HP() {HP--;}
        void inc_HP() {HP++;}
        int get_HP() {return HP;}
        void refillHP() {HP = DOODLEBUG_HP;}
        bool strvation() {
            return !HP;
        }
    private:
        int HP = DOODLEBUG_HP;
};

class Plant : public Organism {
    public:
        using Organism::Organism;
        Plant() : 
        Organism(PLANT_SYMBOL) {
            //Predation.push_back(ANT_SYMBOL);
            reproduce_cycle = PLANT_REPRODUCE_CYCLE;
        }
        Plant(int x, int y) : 
        Organism(x, y, PLANT_SYMBOL) {
            //Predation.push_back(ANT_SYMBOL);
            reproduce_cycle = PLANT_REPRODUCE_CYCLE;
        }
        Matter* childbirth(int x, int y) {
            info.add(PLANT_SYMBOL);
            return new Plant(x,y);
        }
        int go_where(Space *space, int oldPos) {
/*            int pos = get_newPos(space, oldPos, PLANT_SYMBOL);
            if(pos == -1) {
                //fail to eat
                --HP;
               return get_newPos(space, oldPos, UNIVERSAL_SYMBOL);
            } else {
                refillHP();
                return pos;
            }*/
            //--HP;
            refillHP();
            return -1;
        }
        void refillHP() {
            //if(HP < PLANT_HP) {
            //    ++HP;
            //}
        }
        //bool strvation() {
         //   return !HP;
        //}
        void dec_HP() {HP--;}
        void inc_HP() {HP++;}
        int get_HP() {return HP;}
    private:
        int HP = PLANT_HP;
};



void signal_callback_handler(int signum) {
    _enable_cursor();
   // Terminate program
   exit(signum);
}


int main(int argc, char** argv) 
{
//    int xlimit = 20;
 //   int ylimit = 20;
  //  int doodlebug_num = 100;
   // int ant_num = 120
    signal(SIGINT, signal_callback_handler);


    cxxopts::Options options("./exe", "opeions");

    options.add_options()
        ("x,xlimit", "width size of the world", cxxopts::value<int>()->default_value("20"))
        ("y,ylimit", "height size of the world", cxxopts::value<int>()->default_value("20"))
        ("d,doodlebug_num", "number of doodlebugs", cxxopts::value<int>()->default_value("100"))
        ("a,ant_num", "number of ants", cxxopts::value<int>()->default_value("120"))
        ("p,plant_num", "number of plants", cxxopts::value<int>()->default_value("100"))
        ("h,help", "Print usage")
    ;
    auto result = options.parse(argc, argv);
    if (result.count("help")){
      std::cout << options.help() << std::endl;
      exit(0);
    }


    int xlimit = result["xlimit"].as<int>();
    int ylimit = result["ylimit"].as<int>();
    int doodlebug_num = result["doodlebug_num"].as<int>();
    int ant_num = result["ant_num"].as<int>();
    int plant_num = result["plant_num"].as<int>();



    Space space(xlimit, ylimit); //ok
    Doodlebug doodlebug;
    Ant ant;
    Plant plant;

    space.gen_matter(&doodlebug, doodlebug_num); //ok
    space.gen_matter(&ant, ant_num); //ok
    space.gen_matter(&plant, plant_num); //ok

    info.count_num_show();

    space.run();

    cout << "delete all" << endl;

    return EXIT_SUCCESS;


}
