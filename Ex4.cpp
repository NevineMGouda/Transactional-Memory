
// exceptions
#include <iostream>
using namespace std;
int calculate_age(int birth_year){
  int current_year = 2017;
  return current_year-birth_year;

}
int main () {
  int birth_year, age, ticket_price;

  cout<<"Please enter your birth year: ";
  cin>>birth_year;
  try {
    __transaction_atomic {

      age=calculate_age(birth_year);
      if (age<0){
        ticket_price=-100;
        __transaction_cancel throw 1;}
      if (age<=3){ticket_price = 0;}
      else if(age<=11){ticket_price = 50;}
      else if(age<=19){ticket_price = 200;}
      else if(age<=35){ticket_price = 400;}
      else if(age<=50){ticket_price = 500;}
      else if(age<=100){ticket_price = 100;}
      else{ticket_price = 0;}
    }
  }
  catch (int &e) {
    // ticket_price=-1;
    cout<<"Birth year entered is bigger than 2017"<<endl;
    // return 0;
  }
  cout<<"Age: "<<age<<endl;
  cout<<"ticket_price= "<<ticket_price<<endl;

  return 0;
}
