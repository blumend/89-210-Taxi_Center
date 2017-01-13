#include <iostream>
#include "ConnectGrid.h"
#include <queue>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <sstream>
#include <cstring>
#include "BFS.h"
#include "Driver.h"
#include "TaxiCenter.h"
#include "LuxuryCab.h"
#include "StandardCab.h"
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "Udp.h"
#include "Server.h"
#include <unistd.h>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
using namespace std;

/**
 * taking details about driver in the input and creating new driver
 * @return new driver
 */
Driver* inputDriver(char* buffer) {
    Driver *driver = new Driver();

    int id, age, yoe, cabId;
    char dummy, marital;
    char* token;
    int i = 1;

    token = strtok (buffer,",|");
    while (token != NULL) {
        switch (i)
        {
            case 1:
                driver->setId(atoi(token));
                break;
            case 2:
                driver->setAge(atoi(token));
                break;
            case 3:
                driver->setMaritalStatus(token[0]);
                break;
            case 4:
                driver->setYearsOfExperience(atoi(token));
                break;
            case 5:
                driver->setCabId(atoi(token));
                break;
        }
        i++;
        token = strtok(NULL, ",|");
    }
    return driver;
}

/**
 * taking details about trip in the input and creating new trip
 * @return new trip
 */
Trip* inputTrip() {
    Trip *trip = new Trip();
    int id, xStart, yStart, xEnd, yEnd, numOfPassengers;
    double tariff;
    int timeOfStart;
    char dummy;
    cin >> id >> dummy >> xStart >> dummy >> yStart >> dummy >> xEnd >> dummy >> yEnd >> dummy >>
        numOfPassengers >> dummy >> tariff >> dummy >> timeOfStart;
    trip->setRideId(id);
    trip->setStartX(xStart);
    trip->setStartY(yStart);
    trip->setEndX(xEnd);
    trip->setEndY(yEnd);
    trip->setNumberOfPassengers(numOfPassengers);
    trip->setFare(tariff);
    trip->setTimeOfStart(timeOfStart);
    return trip;
}

/**
 * taking details about cab in the input and creating new cab
 * @return new cab
 */
Cab* inputCab(std::list<string> &cabserialize) {
    Cab *cab;
    int id, type;
    char brand, color, dummy;
    cin >> id >> dummy >> type >> dummy >> brand >> dummy >> color;
    string str = boost::lexical_cast<string>(id) + "," + boost::lexical_cast<string>(type) + "," + brand
                 + "," + color + "|";
    cabserialize.push_back(str);
    if (type == 1)
        cab = new StandardCab(id, type, brand, color);
    else
        cab = new LuxuryCab(id, type, brand, color);
    return cab;
}
/**
 *
 * @return main function return 0.
 */
int main(int argc, char *argv[]) {
    Server server(atoi(argv[1]));
    server.Start();
    int currentTime = 0;
    int gridSize[2];
    int numOfObstacles;
    int i = 0;
    int numOfDrivers; //num of drivers to create
    std::list<Point> obstacles;
    char assignBuffer[7];
    std::list<Driver*> drivers;
    std::list<Cab*> cabs;
    std::list<string> cabserialize;
    TaxiCenter center = TaxiCenter(&drivers, &cabs);
    Point point = Point();
    char option[2];
    std::queue<Trip*> tripQueue;
    stack<Point> path;
    int driverId;
    Driver *driver;
    list<Driver*>::iterator it;
    char buffer[1024];
    char buffer2[1024];
    list<string>::iterator serializeIt;
    Driver *driver2;
    string locationSerialize;
    Location* loc;

    //the grid input
    cin >> gridSize[0] >> gridSize[1];
    cin >> numOfObstacles;
    //creating obstacles if the num of them is bigger than 0.
    if(numOfObstacles > 0) {
        for (i = 0; i < numOfObstacles; i++) {
            cin >> point;
            obstacles.push_back(point);
        }
    }
    //creating locations for building a grid
    Location** locations = new Location*[gridSize[0]];
    for(int i = 0; i < gridSize[0]; ++i)
        locations[i] = new Location[gridSize[1]];
    for (int j = 0; j < gridSize[0]; ++j) {
        for (int i = 0; i < gridSize[1]; ++i) {
            locations[j][i].setParent(NULL);
            locations[j][i].setDistance(1000);
            locations[j][i].setPoint(Point(j,i));
        }
    }
    //creating a grid
    Grid grid = Grid(locations, obstacles, gridSize[0], gridSize[1]);

    //menu's option of the user
    cin>>option;
    socket->sendData(option);
    while ( option[0] != '7' ) {
        switch (option[0]) {
            //creating a new driver
            case '1':
                cin >> numOfDrivers;
                while (numOfDrivers > 0) {

                    int cabId;
                    socket->reciveData(buffer, sizeof(buffer));
                    driver2 = inputDriver(buffer);
                    drivers.push_back(driver2);
                    center.assignCabToDriver();
                    --numOfDrivers;
                    socket->sendData(buffer2);
                }
//                cabId = driver->getCabId();
                break;
                //creating new trip
            case '2': {
                tripQueue.push(inputTrip());
            }
                break;
                //creating new cab
            case '3':
                cabs.push_back(inputCab(cabserialize));
                serializeIt = cabserialize.begin();
                for (i = 0; i < cabserialize.size(); ++i) {
                    strcpy(buffer2, (*(serializeIt)).c_str());
                    std::advance(serializeIt, 1);
                }

                break;
                //printing location of a driver according to id
            case '4':
                cin >> driverId;
                it = drivers.begin();
                for (int i = 0; i < drivers.size(); ++i) {
                    if ((*it)->getId() == driverId) {
                        cout << (*it)->getCab()->getLocation().getPoint();
                    }
                    std::advance(it, 1);
                }

                break;
                //all the drivers are driving to their destination point
            case '6':
                /*it = drivers.begin();
                //assign trip to driver
                for (int i = 0; i < drivers.size(); ++i) {
                    if ((*it)->getCab()->getTrip() == NULL && !tripQueue.empty()) {
                        (*it)->getCab()->setTrip(tripQueue.front());
                        tripQueue.pop();
                    }
                    std::advance(it, 1);
                }
                it = drivers.begin();
                //making driver drive
                for (int i = 0; i < drivers.size(); ++i) {
                    (*it)->getCab()->Drive();
                    (*it)->getCab()->setTrip(NULL);
                    std::advance(it, 1);
                }*/

                break;
            case '9':

                it = drivers.begin();
               socket->reciveData(assignBuffer,sizeof(assignBuffer));
                //assign trip to driver
                if (!tripQueue.empty() /*&& strcmp(assignBuffer, "assign") == 0*/) {
                    for (int i = 0; i < drivers.size(); ++i) {
                        if ((*it)->getCab()->getTrip() == NULL && !tripQueue.empty()){
                            (*it)->getCab()->setTrip(tripQueue.front());
                            tripQueue.pop();
                        }
                        std::advance(it, 1);
                    }
                } else {
                    for (int i = 0; i < drivers.size(); ++i) {
                        loc = new Location((*(it))->getCab()->getTrip()->getStartPoint(), NULL);
                        ConnectGrid::connect(grid, loc, grid.getRow(), grid.getCol());
                        path = BFS::bfs(grid,(*(it))->getCab()->getTrip()->getStartPoint(), (*(it))->getCab()->getTrip()->getEndPoint());
                        if(!path.empty()) {
                            if ((*(it))->getCab()->getTrip()->getTimeOfStart() == currentTime) {
                                if ((*(it))->getCab()->getType() == 2 && path.size() > 1) { // luxury cab drives through 2 points each time
                                    (*it)->getCab()->Drive(path.top());
                                    (*(it))->getCab()->getTrip()->setStartPoint(path.top());
                                    path.pop();
                                }
                                (*it)->getCab()->Drive(path.top());

                                (*(it))->getCab()->getTrip()->setStartPoint(path.top());
                                path.pop();

                                locationSerialize = boost::lexical_cast<string>((*(it))->getCabId()) + "," +
                                                    boost::lexical_cast<string>(
                                                            (*it)->getCab()->getLocation().getPoint()) +
                                                    "," + boost::lexical_cast<string>(
                                        (*it)->getCab()->getLocation().getDistance());
                                (*it)->getCab()->getTrip()->setTimeOfStart((*it)->getCab()->getTrip()->getTimeOfStart() + 1);
                                if(path.empty())
                                    (*it)->getCab()->setTrip(NULL);

                            }
                            socket->sendData(locationSerialize);
                        } else {
                            (*it)->getCab()->setTrip(NULL);
                        }
                        std::advance(it, 1);
                    }

                }
                currentTime++;

                break;
        }
        cin>>option;
        socket->sendData(option);
    }


    // support more than one client?
    return 0;
}
