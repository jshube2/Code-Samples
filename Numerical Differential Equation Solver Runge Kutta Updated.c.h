/* 
Author: Joshua Shubert 
Version: 2.0.0
Revision Date: 7/22/2016
Description: This program numerically solves the ordinary differential equation y' = x - y^2 using 
the Runge-Kutta Method. The Runge Kutta method asks for a step-size "h" and two initial values, one for "x" 
and one for "y" to solve for the "k" valued increments for y. The initial values uniquely define the 
differential equation where y' = x -y^2 is a family of DEs. In practice these can be found with knowledge
about what the DE is modelling.
For further information on this method, see: https://en.wikipedia.org/wiki/Runge%E2%80%93Kutta_methods
Update Description: I am rewriting this code to show that how I write code has improved.
*/

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <math.h>

#define Q 0.1667;

using namespace std;

void inputManagerRK(double &x, double &y, double &h)
{
	printf("Runge-Kutta Method:\n");
	printf("Enter a value for the precision of the approximation.\n");
	cin>>h;
	printf("Enter the X value for the initial condition.\n");
	cin>>x;
	printf("Enter the Y value for the initial condition.\n");
	cin>>y;
	printf("--------------------------------------------------------------------------------\n");
}

void rungeKutta(double x, double y, double h, double * f, double * k)
{
	// Calculates and outputs the k increments of the function y' = x - y^2
	// using the Runge Kutta method
	// 10 iterations of this method are calculated
	
	printf("Now calculating the k value increments:\n");
	
	for (int i=0; i<=10; i++)
	{	
		printf("n=%d ",i);
		printf("x=%f.4 y=%f.4 ",x,y);

		f[0]=x-pow(y,2);
		k[0]=h*f[0];
		
		printf("f(x,y)=%f.4\n\n",f[0]);

		f[1]=x-pow(y+0.5*k[0],2);
		k[1]=h*f[1];

		f[2]=x-pow(y+0.5*k[1],2);
		k[2]=h*f[2];

		f[3]=x-pow(y+k[2],2);
		k[3]=h*f[3];

		printf("k1=%f.4 k2=%f.4 k3=%f.4 k4=%f.4\n\n",k[0],k[1],k[2],k[3]);
		
		y=y+(Q*(k[0]+(2.0*k[1])+(2.0*k[2])+(k[3])));
		x=x+h;
	}
	printf("Calculations complete!\n");
	printf("--------------------------------------------------------------------------------\n");
}
int main ()
{
	// Define variables
	double h,x,y;
	double f[4];
	double k[4];

	// Get necessary input values
	inputManagerRK(x,y,h);
	
	// Perform rungeKutta method with the aforementioned input
	rungeKutta(x,y,h,f,k);

	system("pause");
	return 0;
}
