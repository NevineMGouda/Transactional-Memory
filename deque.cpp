#include <atomic>
#include <iostream>
#include <thread>
#include <unistd.h>

class DQueue
{
  struct QNode
  {
    QNode *left, *right;
    int val;
  };

  QNode *leftSentinel, *rightSentinel;


public:

  DQueue()
  {
    leftSentinel = nullptr;
    rightSentinel = nullptr;
  }

  QNode* newQNode(int val, QNode* left, QNode* right) {
    QNode* tmp = new QNode();
    tmp->val = val;
    tmp->left = left;
    tmp->right = right;
  }

  void PushLeft(int val)
  {
    QNode* tmp = new QNode(); // = newQNode(val, nullptr, nullptr)
    tmp->val = val;
    tmp->left = nullptr;
    tmp->right = nullptr;
    __transaction_atomic
    {
      if ( isEmpty() ) {
          // Add the first element
	  // QNode* tmp = newQNode(val, nullptr, nullptr)
          leftSentinel = rightSentinel = tmp;
      }
      else {
          // Prepend to the list and fix links
	  // QNode* tmp = newQNode(val, nullptr, leftSentinel)
          tmp->right = leftSentinel;
          leftSentinel->left = tmp;
          leftSentinel = tmp;
      }
    }


  }

  void PushRight(int val)
  {
    QNode* tmp = new QNode();
    tmp->val = val;
    tmp->left = nullptr;
    tmp->right = nullptr;
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
    }
  }

  int PopLeft()
  {
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

      return ret;
    }
  }

  int PopRight()
  {
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

      return ret;
    }
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
    //std::cout << "random printing " << leftSentinel << std::endl;
    while (tmp!= nullptr){
      std::cout<<"Value:"<<tmp->val<<std::endl;
      //std::cout<<tmp->val << ", ";
      tmp = tmp->right;
    }
  }


};
void runPushLeft(DQueue *q, int x){
  //std::cout<<x<<std::endl;
  q->PushLeft(x);
}
void runPushRight(DQueue *q, int x){
  //std::cout<<x<<std::endl;
  q->PushRight(x);
}
void runPopLeft(DQueue *q){
  q->PopLeft();
}
void runPopRight(DQueue *q){
  q->PopRight();
}
/*int main()
{
    // Stack behavior using a general dequeue
    DQueue q;
    int threads_count = 8;
    std::thread *t = new std::thread[threads_count];

    for(int i=0 ; i< threads_count;i++){
      t[i] = std::thread(runPushLeft, &q, i);}
    for(int j=0; j<threads_count;j++){
    	t[j].join();
    }
    q.TraverseLeft();


    return 0;

};*/
