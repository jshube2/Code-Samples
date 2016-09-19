/* Note: This program was originally written in spring 2014 as an extra credit project
that I took on in my Differential Equations class. I recieved points for it because it
produced the correct output, however as I have matured it has become an example of 
poor style. I have since updated the style and formatting of this program. See version 2.0.0 or
greater. */

// Joshua Shubert 
// This program numerically solves a differential equation using the Runge-Kutta Method.

#include <iostream>
#include <iomanip>

using namespace std;

int main ()
{
	double h,x,y,f,f1,f2,f3,f4,k1,k2,k3,k4,Q;

	Q= (0.1667);

	cout<<"Runge-Kutta Method:\n";
	cout<<"Enter a value for the precision of the approximation.\n";
	cin>>h;
	cout<<"Enter the X value for the initial condition.\n";
	cin>>x;
	cout<<"Enter the Y value for the initial condition.\n";
	cin>>y;
	cout<<"--------------------------------------------------------------------------------\n";

	for (int i=0; i<11; i++)
	{
		std::cout<<std::setprecision(4)<<std::fixed;
		
		cout<<"n="<<i<<" ";
		cout<<"x="<<x<<" y="<<y<<" ";

		f=x-pow(y,2);
		k1=h*f;
		cout<<"f(x,y)="<<f<<endl<<endl;

		f2=x-pow(y+0.5*k1,2);
		k2=h*f2;

		f3=x-pow(y+0.5*k2,2);
		k3=h*f3;

		f4=x-pow(y+k3,2);
		k4=h*f4;

		cout<<"k1="<<k1<<" k2="<<k2<<" k3="<<k3<<" k4="<<k4<<endl<<endl;
		y=y+Q*(k1+2*k2+2*k3+k4);
		x=x+h;
		
		
	}
	cout<<"--------------------------------------------------------------------------------\n";
	
	system("pause");
	return 0;
}