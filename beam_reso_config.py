from matplotlib import pyplot as mp
import numpy as np
import math as mt 

#run parameters (how long are we running?)
time = 1.0 #in seconds

#energy spread range 
min_value = 2.0
max_value = 2.4
ADC_channels = 1024  #tbc with compton guys
#derived variables (change only if you know what you're doing!)
central_energy_value = (min_value + max_value)/2 
energy_per_channel = (max_value - min_value)/ADC_channels

#beam parameters
#Total number of photons per beam (integral_-inf_+inf = sigma*sqrt(2pi)*I_max )
N_total = 1.0e10
#width of the energy distribution (sigma of the Gauss function)
width = 0.005
#diametral width
d_width = 100 #mm2 (1cm?)
#derived variables (change only if you know what you're doing!)
W_beam = central_energy_value*width
I_max = N_total/(W_beam/energy_per_channel*mt.sqrt(2*mt.pi))
E_step_size = 4*W_beam/10 #~10% energy spread (pm 2 sigma)
#E_step_size = 0.001*central_energy_value
#make 20 scans around e_central
min_e = central_energy_value-10*E_step_size
max_e = central_energy_value+10*E_step_size


#resonance parameters (more than one reso in range possible)
E_reso = [2.2] #es.
W_reso = [8.2e-6]  #in MeV
I_reso = [64e-24] #peak cross-section in cm2 (barn conversion)
#int_xsec = [8e-28] #in cm2*MeV

#I_reso = []
#for i in range(0, len(E_reso)):
#	I_reso.append(int_xsec[i]/(W_reso[i]/energy_per_channel*mt.sqrt(2*mt.pi)))

#derived variables (change only if you know what you're doing!)
#ADC channel of the resonance
resonance_index = []
for i in range(0, len(E_reso)):
	resonance_index.append(ADC_channels*(E_reso[i]-min_value)/(max_value-min_value))

#target parameters 
target_density = 6.1e20 #es. cm^-2
target_tickness = 0.15   #es. cm

#detector parameters (up to now only a rough solid angle and counting efficiency)
dOmega = 0.5 #(15*15-25)cm^2/20cm 
counting_efficiency = 0.00664 #evaluated with 4BaF2 at 2.2MeV

#analysis parameters (cut scans giving counts below a certain treshold)
count_cut = 10000