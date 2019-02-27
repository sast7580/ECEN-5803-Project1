#include "flowfuncts.h"
#include "shared.h"

float sampleAlg(float freq, float temp_degC)
{
    // dependent variables
    float d_inch = 0.5;
    float PID_inch = 2.9;
    float PID_meter = 0.07366;
    float temp_degK = temp_degC + 273.15;
    
    float rho = 1000*(1-((temp_degC+288.9414)/(508929.2*(temp_degC+68.12963)))*(powf(temp_degC-3.9863, 2)));
    float visc = 2.4e-5 * powf(10,(247.8/(temp_degK-140.0)));
    
    // calculate velocity
    float a = powf(0.2684, 2);
    float b = -(powf(1.0356,2))/(rho*PID_meter/visc);
    float c = -powf(freq*d_inch, 2);
    
    float velocity = quadratic(a, b, c);    
    float volumetricFlow = 2.45*powf(PID_inch, 2) * velocity;
    
    return volumetricFlow;
};


float quadratic(float a, float b, float c)
{
    float d = b*b - 4*a*c;
    float root1, root2;
    
   if (d < 0) { //complex roots
     printf("First root = %.2lf + j%.2lf\n", -b/(double)(2*a), sqrt(-d)/(2*a));
     printf("Second root = %.2lf - j%.2lf\n", -b/(double)(2*a), sqrt(-d)/(2*a));
   }
   else { //real roots
      root1 = (-b + sqrt(d))/(2*a);
      root2 = (-b - sqrt(d))/(2*a);
    }
    
    if(root1 < 0)
    {
        return root2;
    }
    else
    { 
        return root1;
    }
};
