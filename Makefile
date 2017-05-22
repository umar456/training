
solutions:
	make -C lab-01-increment/solution
	make -C lab-02-vector-addition/solution
	make -C lab-03-pi/solution

check:
	make -C lab-01-increment/solution check
	make -C lab-02-vector-addition/solution check
	make -C lab-03-pi/solution check
clean:
	make -C lab-01-increment/solution clean
	make -C lab-02-vector-addition/solution clean
	make -C lab-03-pi/solution clean
