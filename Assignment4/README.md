				REVERSE ENGINEERING CPU CACHES
				
Objective : To identify the cache block size and the associativity of the L1 cache.


Steps to compile the c files:

	1. First we need to isolate certain cpu's so that we can allocate these for our program
	   only. For this we have to type the command "isolcpus=0,1" in the GRUB and reboot.
	   
	2. The above command makes sure that the cpu's numbered 0,1 are isolated for our purpose.
	   This can be verified by using the command "cat /sys/devices/system/cpu/isolated"
	   
	3. Now for compilation use the following commands in that order:
	
			"gcc filename.c -o execfile"
			"taskset -c 1 ./execfile"
	    
	   Fill the 'filename' with appropriate filename. The first command creates and executable
	   file named "execfile". The second command executes the execfile and also makes sure that
	   our program runs on cpu 1 which is isolated.

