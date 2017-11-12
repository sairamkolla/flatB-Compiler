declblock{
	int i, n,a,b,c;
}

codeblock{
	read n;
	a = 1;
	b = 1;
	if(n>=1){
		println a;
	}
	if(n>=2){
		println b;
	}
	n = n-2;
	while(n>0){
	
		c = a + b;
		a = b;
		b = c;
		println b;
		n = n-1;
	}
	

}
