#import configuration file 
from beam_reso_config import *

print "--- Energy scan ---"
print "min_value (MeV) = ", min_value
print "max_value (MeV) = ", max_value
print "Number of ADC_channels = ",ADC_channels
print "(Energy per channel = ",energy_per_channel,")"
print "Average step size = ",E_step_size
print "21 scans will be performed with mean energies ranging from ",min_e," to ",max_e," MeV"
print "\n"
print "--- Beam parameters ---"
print "peak brillance (N_gamma*s-1*mm-2) = ",N_total
print "beam width = ",W_beam,"(",width*100,"%)"
print "\n"
print "--- Resonance parameters ---"
print "The following resonances are in range"
print "Energy (MeV) = \t", E_reso
print "@ADC ch = \t", resonance_index
print "width (MeV) = \t", W_reso
print "Peak cross-section (cm-2) = \t",I_reso
print "\n"
print "--- Target parameters ---"
print "Density = ",target_density
print "tickness (cm) = ",target_tickness
print "\n"
print "--- Detector parameters ---"
print "solid angle (rad) = ", dOmega
print "counting efficiency = ", counting_efficiency
print "\n"
print "======================="
print "\n"

#define the two gauss functions
def gaussian(x, high, mu, sig):
	return high*np.exp(-np.power(x-mu, 2.) / (2 * np.power(sig, 2.)))

E = np.arange(min_value, max_value, (max_value - min_value)/ADC_channels) #sample with ADC channels

#fine energy range for resonance 
E_r = []
for i in range(0, len(E_reso)):
	E_r.append(np.arange(E_reso[i]-10*W_reso[i], E_reso[i]+10*W_reso[i], 20*W_reso[i]/ADC_channels))

#define the "perfect runner" fixed steps
E_fixed_step = np.arange(min_e, max_e, E_step_size)


int_perfect_list = []

#define the template "perfect runner" function
for E_central in E_fixed_step:
	f = gaussian(E, I_max*time, E_central, W_beam)
	g = 0
	for i in range(0, len(E_reso)):
		g = g + gaussian(E_r[i], I_reso[i], E_reso[i], W_reso[i])
	h = f*g

	#evaluation of the number of counts per shot
	Inte = np.trapz(h)*target_density*target_tickness*dOmega/(4*mt.pi)*d_width*counting_efficiency #integral

	int_perfect_list.append(Inte)

	#mp.plot(f)
	#mp.plot(g)
#mp.show()

n_rand_steps = len(int_perfect_list)


pos_er_list = []
neg_er_list = []

for j in range(0,10):
	mu, sigma = E_step_size, 0.003 # mean and standard deviation
	s = np.random.normal(mu, sigma, n_rand_steps)

	E_random_step = [min_e-E_step_size+s[0]]
	for p in range(1,n_rand_steps):
		E_random_step.append(E_random_step[p-1]+s[p])

	min_list = [999999999]*ADC_channels
	max_list = [0]*ADC_channels
	max_error_list = [0]*ADC_channels
	min_error_list = [0]*ADC_channels

	g_integral = []
	for i in range(0, len(E_reso)):
		g = gaussian(E_r[i], I_reso[i], E_reso[i], W_reso[i])
		#mp.plot(g)
		g_integral.append(np.trapz(g))

	int_rand_list = []
	#mp.plot(g*5e30)
	for E_central in E_random_step:
	#for E_central in (E_random_step[7],E_random_step[9],E_random_step[10],E_random_step[11],E_random_step[13],E_random_step[15]):
		
		f = gaussian(E, I_max*time, E_central, W_beam)
		#g = gaussian(E, I_reso, E_reso, W_reso)
		#mp.plot(f)
		#h = f*g
		#Inte = np.trapz(h)*target_density*target_tickness*dOmega/(4*mt.pi)*d_width*counting_efficiency  #integral
		Inte0 = 0
		for i in range(0, len(E_reso)):
			Inte0 = Inte0 + g_integral[i]*f[resonance_index[i]]

		Inte = Inte0*target_density*target_tickness*dOmega/(4*mt.pi)*d_width*counting_efficiency

		#print "func f (integral) = ", np.trapz(f)," func g (integral) = ", np.trapz(g)
		#print "Integral = ", Inte0, " * ", target_density*target_tickness*dOmega/(4*mt.pi)*d_width*counting_efficiency, "--> counts = ",Inte
		int_rand_list.append(Inte)

		int_err = mt.sqrt(Inte)

		ratio_list = []
		#record only events with reso counters > 10000 (error O(1%))
		if Inte >=count_cut:
			for channel in range(0,ADC_channels):
				point_energy = E[channel]
				ADC_count = f[channel]#*(max_value - min_value)/ADC_channels
				ADC_err = mt.sqrt(ADC_count)

				if ADC_count < 1:
					ADC_count = 0
				#print "ch: ", channel, " = ",ADC_count
				ratio_list.append(ADC_count/Inte)
				#only in central region (tbc)
				if ADC_count/Inte > max_list[channel]:
					max_list[channel] = ADC_count/Inte
					if ADC_count!=0 and Inte!=0:
						max_error = max_list[channel]*(1/mt.sqrt(ADC_count)+1/mt.sqrt(Inte))
					else:
						max_error = 0
					max_error_list[channel] = max_error
				if ADC_count/Inte < min_list[channel]:
					min_list[channel] = ADC_count/Inte
					if ADC_count!=0 and Inte!=0:
						min_error = min_list[channel]*(1/mt.sqrt(ADC_count)+1/mt.sqrt(Inte))
					else:
						min_error = 0
					min_error_list[channel] = min_error
				#ratio_list.append(0) 
			#mp.plot(ratio_list)

	deltas = []
	error_list = []
	deltamin = []
	deltamax = []

	for channel in range(0,ADC_channels):
		delta = max_list[channel] - min_list[channel]   
		deltas.append(delta)
		delta_error = mt.sqrt(min_error_list[channel]*min_error_list[channel] + max_error_list[channel]*max_error_list[channel])
		error_list.append(delta_error)
		deltamin.append(delta - delta_error)
		deltamax.append(delta + delta_error)


	#found_channel = deltamin.index(min(deltamin))

 	lower_ch = int(round((min_e-min_value)/energy_per_channel))
 	higher_ch = int(round((max_e-min_value)/energy_per_channel))

 	reduced_list = []

 	for s in range(0,(higher_ch - lower_ch)):
 		reduced_list.append(deltas[s+lower_ch])

	found_channel = lower_ch + reduced_list.index(min(reduced_list))
	found_max = deltamax[found_channel]

	#find the extremals
	int_mover = found_channel

	#print found_channel

	if found_channel <(min_e-min_value)/energy_per_channel or found_channel>(max_e-min_value)/energy_per_channel:
		print "Error in finding channel"
	else:
		while deltamin[int_mover] <= found_max:
			int_mover+=1
		pos_error = int_mover - found_channel

		int_mover = found_channel

		while deltamin[int_mover] <= found_max:
			int_mover-=1
		neg_error = found_channel - int_mover 

		print "Minimum at: ", found_channel, " + ", pos_error, "/ - ", neg_error
		pos_er_list.append(pos_error)
		neg_er_list.append(neg_error)
		

	#mp.plot(deltamax)
	mp.plot(deltas)
	#mp.plot(deltamin)

#endif j

averageminer = 0
averagemaxer = 0
if len(neg_er_list) != 0 and len(pos_er_list) != 0:
	averageminer = sum(neg_er_list)/len(neg_er_list)
	averagemaxer = sum(pos_er_list)/len(pos_er_list)
	
print "Count cut = ", count_cut, "Average channel indetermination = ",averagemaxer," - ",averageminer, "Total = ",averagemaxer+averageminer
#mp.yscale('log')

mp.xlabel("ADC channel")
mp.ylabel("(max - min)_ch")
mp.suptitle("Resonance finder function for %d different scans" % (j+1))
mp.text(720, 10000,'ADC_channels = %d\nCount_cut = %d\nResonance position = ch%d\nBeam_width = %f' % (ADC_channels , count_cut , resonance_index[0] , width))
#mp.savefig('%dADC_%dcc_%dResPos.pdf' % (ADC_channels , count_cut , resonance_index[0]), bbox_inches='tight')
mp.show()