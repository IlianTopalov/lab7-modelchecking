/******************************************************************************
Fichier: exemple.c
Auteur:  Yann Thoma
Date:    05.05.09.
Description: Tampon simple.
    Les producteur doivent attendre qu'un consommateur ait lu
    la donnée avant de pouvoir en remettre une nouvelle.
******************************************************************************/


#include <iostream>
#include <semaphore.h>

#include <pcosynchro/pcothread.h>
#include <pcosynchro/pcosemaphore.h>

#include "buffern.h"

using namespace std;

#define NB_THREADS_CONSUMER   4
#define NB_THREADS_PRODUCER   2

#define ITEM int


static AbstractBuffer<ITEM> *buffer;

[[noreturn]] void producerTask() {
  ITEM item = 0;
  while (true) {
    // produire item
    cout << "Tache producteur dépose" << endl;
    buffer->put(item);
    cout << "Tache producteur a déposé" << endl;
    PcoThread::usleep((int)((float)6000000*rand()/(RAND_MAX+1.0)));
  }
}

[[noreturn]] void consumerTask() {
  ITEM item;
  while (true) {
    cout << "                                  Tache consommateur attend" << endl;
    item = buffer->get();
    cout << "                                  Tache consommateur a récupéré" << endl;
    // consommer item

    PcoThread::usleep((int)((float)6000000*rand()/(RAND_MAX+1.0)));
  }
}

int main(void) {
  std::vector<PcoThread*> producers;
  std::vector<PcoThread*> consumers;
  /*
    switch (test) {
    case 0: buffer = new Buffer1<ITEM>();break;
    case 1: buffer = new Buffer1a<ITEM>();break;
    case 2: buffer = new BufferN<ITEM>(4);break;
    case 3: buffer = new BufferNa<ITEM>(4);break;
    }

    */
  buffer = new BufferN<ITEM>(10);
  for(int i=0; i<NB_THREADS_PRODUCER; i++)
      producers.push_back(new PcoThread(producerTask));
  for(int i=0; i<NB_THREADS_CONSUMER; i++)
      consumers.push_back(new PcoThread(consumerTask));
  for(size_t i=0; i<NB_THREADS_PRODUCER; i++)
    producers[i]->join();
  for(size_t i=0; i<NB_THREADS_CONSUMER; i++)
    consumers[i]->join();
  return 0;
}
