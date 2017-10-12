declblock{
int a[5];
int temp;
int i,j;
bool flag;
}

codeblock{
a[0] = 4;
a[1] = 2;
a[2] = 5;
a[3] = 3;
a[4] = 1;
flag=true;
if(a[2]==53){
	print "Inside the if statement";
}else{
	print "Inside the else statemetn";
}

for i=0,4{
	for j=0,4{
		if ( a[i] < a[j] ) {
			temp = a[i];
			a[i] = a[j];
			a[j] = temp;
		}	
	}	
}

for i=0,4{
	print a[i],"";
}
println "";

}

