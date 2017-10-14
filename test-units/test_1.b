declblock{
	int a,b,c;
	int d[5];
}

codeblock{
	a = 5+4+5;
	b = 6*7;
	if(a>4){
		a = a+10;
	}
	println "value of a is ",a;
	for i=1,2,10{
		a = a+1;
		println "value of a is ",a;
	
	}
	while(a>20){
	a = a-1;
	read a;
	println "value of a is ",a;
	}
}
