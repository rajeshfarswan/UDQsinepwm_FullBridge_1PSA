//********************************************************************************//
//*************STAND ALONE SINGLE PHASE FULL-BRIDGE INVERTER**********************// 
//*************BASED ON SINE PWM USING UNBALANCED DQ CONTROL**********************//
//********************************************************************************//


#include "main.h"    // Include file containing processor registors definitions and function definitions
#include "user.h"    // Include definitions of local variables
#include "asmMath.h" // Include definition of math functions 

int main(void) //main program
{

init();     // call processor initilisation code

starting(); // Before-PWM initiliasation and soft-start of system

  while(1) //main loop 
	{

//current control start//
    if(current_Flag) //20Khz
       {  
          //generate current references for DQ frame //voltage d-q PI output is current ref   
          Dvalue = Vd_FOFout; //set inverter current ref d
          Qvalue = Vq_FOFout; //set inverter current ref q
          asmDQtoABC();       //inverter current refrences to unbalance dq refrences

           //read inverter current
          asm("disi #0x3FFF");
           {
            Ivalue = asmADC(0x0008) - offset; // read ADC channel 8// inverter current feedback  
            
           } 
          asm("disi #0x0000");
           //
           //detect peak current
          if(Ivalue > current_max) fault_Flag = 0; //max current trip value 
          if(Ivalue < current_min) fault_Flag = 0; //min current trip value  
           
            //inverter current PI
            IPreError = currentP_Dout;
            currentP_Dout = asmPIcontroller(Avalue,Ivalue,current_Pgain,current_Igain);

            asmPWM(); //generate Full-bridge inverter duty cycle           
            
                               current_Flag = 0; //reset flag  
                                  } 
//current control end//

//PLL  and inverter voltage control start//        
	if(pll_Flag) //12Khz _PLL_count
		{
           //generate dq references
           asm("disi #0x3FFF");
           Avalue = asmINT_MPQ(qSin,V_ref); //generate inverter  sinusoidal voltage ref 
           asm("disi #0x0000");
           asmABCtoDQ();
           VD_ref = Dvalue; //obtain inverter d and q voltage ref.
           VQ_ref = Qvalue;
           //

          asm("disi #0x3FFF");
           {
          Avalue = asmADC(0x0005) - offset; //adc channel 5 //read inverter voltage  
          
            } 
          asm("disi #0x0000");
          //

          asmABCtoDQ(); //convert current feedback to d-q format

          /*voltage D PI******************************************************/
          IPreError = Vd_PI_out;
          Vd_PI_out = asmPIcontroller(VD_ref,Dvalue,vPI_Pgain,vPI_Igain);
           
         /*voltage Q PI******************************************************/
          IPreError = Vq_PI_out;
          Vq_PI_out = asmPIcontroller(VQ_ref,Qvalue,vPI_Pgain,vPI_Igain);
             
          //voltage D PI filter
          FOF_PreOut = Vd_FOFout; //inverter current ref. in d
          Vd_FOFout = asmFO_Filter(Vd_PI_out,Filter_const_V);

          //voltage Q PI filter
          FOF_PreOut = Vq_FOFout; //inverter current ref. in q
          Vq_FOFout = asmFO_Filter(Vq_PI_out,Filter_const_V);
         
				         pll_Flag = 0; //reset flag
						  }
//PLL and voltage control end//		


//DC-link monitoring and soft start//
		if(ffd_Flag) //0.5Khz   
      		{  
              asmDClink();  //monitor dc link
              SET = 0x0077; //all three switces are enabled

              V_ref++; //initiate soft start

                   if(V_ref >= V_Dsetpoint)
                       { 
                         V_ref = V_Dsetpoint;
                           
                        } 
       		                    ffd_Flag = 0; //reset flag
       							}
//feed forward and soft start end//

    			ClrWdt();
    		}//while end////////////////////////////////////////////////////////

  
		ClrWdt();
	} //main end////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////


//T1 interrupt for oscillator tracking
		void _ISRFAST __attribute__((interrupt, no_auto_psv)) _T1Interrupt(void) //99.88Khz
  			{   
                
                if(osc_Flag)//7.5Khz
                 {
                  //harmonic oscillator for generating Sine and Cos ref. 
                  OSC_F = OSC_Fcentral; //set inverter frequency 50 Hz
                 
                  theta = theta + OSC_F; //increment theta angle
     
         		    if(theta >= theta_2PI) //reset harmonic oscillator
            		    {
           				qSin = 0;
           				qCos = 32440; //0.99 max reset value
           				theta = 0;
              				}
              					else asmHARMONIC();
                    
                      osc_Flag = 0; //reset flag
                      } 
                
     			T1us_Flag = 0; //reset flag
                
   					} //T1 interupt end

///////////////////////////////////////////////////////////////////////

		//fault interrupt for protection
		void _ISR __attribute__((interrupt, no_auto_psv)) _FLTBInterrupt(void)
  			 {
     			PWMenable = 0; //disable pwm
     			SET = 0;       //all switches off
          
     			RL2_ON = 0;    //open all relays
               
     			RL3_ON = 0;      
     			RL4_ON = 0; 
     			RL5_ON = 0;     
  
		fault_Flag = 0;        //reset flag
            
   			}//fault end

//////////////////////////////////////////////////////////////////////

			//initial startup routine before turning on PWM
			void starting(void)
  				{
                    PWM_offset = PWM_PERIOD; //initialise PWM period value
                    PWM_offset_N = -PWM_PERIOD;

					PWM_max = PWM_offset*8; //PI saturation values
					PWM_min = -PWM_offset*8;
					SET = 0;       //initialise PWM control registers
					PWMenable = 0; //reset PWM control register
					 //
					FAULT_ENABLE = 1; //0x000f //reset fault register
					delay(30);  //delay 30ms
					ADC_ON = 1; //turn on ADC
					//precharging init
					RL1_ON = 1;  //precharging enable
					delay(15); //delay 1500ms
					//precharging init ends
					
					offset = asmADC(0x0e0e); //2.5V offset //read adc channel 14
					//
					//initiates startup
					RL1_ON = 0;  //precharging disable
					delay(30); //delay 30ms
					RL2_ON = 1;  //bypass precharging
					delay(30); //delay 30ms
					
					//set pwm
					PWM1 = PWM_offset;
					PWM2 = PWM_offset;
					PWM3 = PWM_offset;
					//SET = 0x0077; //all three switces are enabled
					//
							
					delay(30); //delay 30ms
					//PWM_InterruptEnable = 0;
					PWMenable = 1;
					T1ON = 1;
                    T2ON = 1;
                    T3ON = 1;
                    T4ON = 1;
                    T5ON = 1;
                    
					// 
					  	}//startup routine end

///////////////////////////////////////////////////////////////////////

			











