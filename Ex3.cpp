#include <atomic>
#include <iostream>
#include <thread>
#include <unistd.h>
std::atomic<bool> terminate;

class DQueue
{
  struct QNode
  {
    QNode *left, *right;
    int val;
  };

  QNode *leftSentinel, *rightSentinel;
  int count;
  int op_count;


public:

  DQueue()
  {
    leftSentinel = nullptr;
    rightSentinel = nullptr;
    count = 0;
    op_count = 0;
  }

  void PushLeft(int val)
  {
    QNode* tmp = new QNode();
    tmp->val = val;
    tmp->left = nullptr;
    tmp->right = nullptr;
    // while (!terminate)
    __transaction_atomic
    {
      if ( isEmpty() ) {
          // Add the first element
          leftSentinel = rightSentinel = tmp;
      }
      else {
          // Prepend to the list and fix links
          tmp->right = leftSentinel;
          leftSentinel->left = tmp;
          leftSentinel = tmp;
      }
      count++;
      op_count++;
    }


  }

  void PushRight(int val)
  {
    QNode* tmp = new QNode();
    tmp->val = val;
    tmp->left = nullptr;
    tmp->right = nullptr;
    // while (!terminate)
    __transaction_atomic
    {
      if ( isEmpty() ) {
          // Add the first element
          leftSentinel = rightSentinel = tmp;
      }
      else {
          // Prepend to the list and fix links
          rightSentinel->right = tmp;
          tmp->left = rightSentinel;
          rightSentinel = tmp;
      }
    count++;
    op_count++;
    }
  }

  int PopLeft()
  {
    // while (!terminate)
    __transaction_atomic
      {
      if ( isEmpty() ) {
              return -1;
      }

      //  Data in the front node
      int ret = leftSentinel->val;

      // Delete the front node and fix the links
      QNode* tmp = leftSentinel;
      if ( leftSentinel->right != nullptr )
      {
          leftSentinel = leftSentinel->right;
          leftSentinel->left = nullptr;
      }
      //Only 1 element exists
      else
      {
          leftSentinel = nullptr;
          rightSentinel = nullptr; //should I remove it?
      }
      delete tmp;
      count--;
      op_count++;
      return ret;
    }
  }

  int PopRight()
  {
    // while (!terminate)
    __transaction_atomic
    {
      if ( isEmpty() ) {
              return -1;
      }

      //  Data in the front node
      int ret = rightSentinel->val;

      // Delete the front node and fix the links
      QNode* tmp = rightSentinel;
      if ( rightSentinel->left != nullptr )
      {
          rightSentinel = rightSentinel->left;
          rightSentinel->right = nullptr;
      }
      //Only 1 element exists
      else
      {
          leftSentinel = nullptr;
          rightSentinel = nullptr; //should I remove it?
      }
      delete tmp;
      count--;
      op_count++;
      return ret;
    }
  }
  int get_count()
  {
    return count;
  }
  int get_operations_count(){
    return op_count;
  }
  bool isEmpty()
  {
    __transaction_atomic
    {
      if (leftSentinel==nullptr || rightSentinel==nullptr){return 1;}
      else{return 0;}
    }
  }
  void TraverseLeft(){
    QNode* tmp = leftSentinel;
    while (tmp!= nullptr){
      std::cout<<"Value:"<<tmp->val<<std::endl;
      tmp = tmp->right;
    }
  }

  void freeQueue() {
    while (leftSentinel!= nullptr){
      QNode* tmp = leftSentinel;
      leftSentinel = tmp->right;
      free(tmp);
    }
  }



};
void runPushLeft(DQueue *q){
  int x;
  while (!terminate){
    x = rand()%100;
    // std::cout<<x<<std::endl;
    q->PushLeft(x);
  }

}
void runPushAndPopLeft(DQueue *q){
  int x;
  while (!terminate){
    x = rand()%100;
    // std::cout<<x<<std::endl;
    q->PushLeft(x);
    q->PopLeft();
  }
}

void runPushLeftAndPopRight(DQueue *q){
  int x;
  while (!terminate){
    x = rand()%100;
    // std::cout<<x<<std::endl;
    q->PushLeft(x);
    q->PopRight();
  }

}
int main(int argc, char *argv[])
{
    if( argc != 2) {
      printf("Usage: %s number_of_threads\n", argv[0]);
      return 0;
    }
    // Stack behavior using a general dequeue
    int threads_count = atoi(argv[1]);

// ------------------------ Push Left

    DQueue q1;
    terminate = false;
    std::thread *t1 = new std::thread[threads_count];

    for(int i=0 ; i< threads_count;i++){
       t1[i] = std::thread(runPushLeft, &q1);}
    sleep(1);

    terminate = true;
    for(int j=0; j<threads_count;j++){
    	t1[j].join();
    }
    std::cout<<"Operations per second <<Only PushLeft>>: "<<q1.get_operations_count()<<std::endl;

    q1.freeQueue();

// ------------------------ Push Left and Pop Left

    DQueue q2;
    terminate = false;
    std::thread *t2 = new std::thread[threads_count];

    for(int i=0 ; i< threads_count;i++){
       t2[i] = std::thread(runPushAndPopLeft, &q2);}
    sleep(1);

    terminate = true;
    for(int j=0; j<threads_count;j++){
    	t2[j].join();
    }
    std::cout<<"Operations per second <<PushLeft and PopLeft>>: "<<q2.get_operations_count()<<std::endl;

    q2.freeQueue();

// ------------------------ Push Left and Pop Right

    DQueue q3;
    terminate = false;
    std::thread *t3 = new std::thread[threads_count];

    for(int i=0 ; i< threads_count;i++){
       t3[i] = std::thread(runPushLeftAndPopRight, &q3);}
    sleep(1);

    terminate = true;
    for(int j=0; j<threads_count;j++){
    	t3[j].join();
    }
    std::cout<<"Operations per second <<PushLeft and PopRight>>: "<<q3.get_operations_count()<<std::endl;

    q3.freeQueue();

    return 0;

};
