//user controlled variable definations//

//********************************************************
//Iout = (2.5+-(0.625*Ip/6)V)*(32735/5) 

//VDCout = (Vdc*15/600)V)*(32735/5)
//VACout = (2.5-(Vac*6.8/600)V)*(32735/5)
//transformer ratio 400/173 = 2.3121
//********************************************************

//general variables
int offset = 0;

//PI general variables
int PWM_max = 0; 
int PWM_min = 0;
int PWM_offset = 0;
int PWM_offset_N = 0;
int IPreError = 0;

// harmonic oscillator variables
int qSin = 0;
int qCos = 32440;        //initial value of 0.99

int OSC_F = 0; 
int OSC_Fcentral = 1372; //2*PI*F*T, F = 50Hz //1372
                         // 150samples //Fs/F

long theta = 205800;     //205800
long theta_2PI = 205800; //samples*OSC_Fcentral
                         //205800

//harmonic oscillator PI variables/////////////////////////
int vPI_Pgain = 18;   //HARMONIC PROPORTIONAL GAIN   //18
int vPI_Igain = 3;    //HARMONIC INTEGRAL GAIN       //3
///////////////////////////////////////////////////////////

int Vd_PI_out = 0;
int Vq_PI_out = 0;

//abc to dq variables
int Avalue = 0;

int Dvalue = 0;
int Qvalue = 0;

//Current control PI variables/////////////////////////////////
int current_Pgain = 27;        //CURRENT PROPORTIONAL GAIN //27
int current_Igain = 3;         //CURRENT INTEGRAL GAIN     //7
///////////////////////////////////////////////////////////////
int Ivalue = 0;
int currentP_Dout = 0;
//int currentP_Qout = 0;
int V_ref = 0;

int VD_ref = 0;
int VQ_ref = 0;


//voltage reference///////////////////////////////////////////////////
int V_Dsetpoint = 7420; //7420 == 100Vpeak
//////////////////////////////////////////////////////////////////////

int DCLINK_MAX = 30800; //180V max//30800

//current protection variables
int current_max =  5440;  //+8A Ipeak limit //5440
int current_min = -5440;  //-8A Ipeak limit //-5440

//first order filter variables//////////////////////
int FOF_PreOut = 0;
int Filter_const_V = 273; // T/tau // 100HZ/12Khz
////////////////////////////////////////////////////

//int ffd_FOFout = 0;
int Vd_FOFout = 0;
int Vq_FOFout = 0;













