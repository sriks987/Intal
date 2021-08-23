#include<stdlib.h>
#include<string.h>
#include"intal.h"

// Following Little Endian representation 

typedef struct number{
	int len;
	int size;
	char *num;
}number;

void remove_zeroes(number* intal){
	int i = intal->len-1;
	while(i>0 && intal->num[i]==0)
		i--;
	intal->len = i+1;
}

void* intal_create(const char* str){
	int k,x;
	int j = 0;
	int n = strlen(str);
	if(str == NULL)
		return NULL;
	number *intal = (number*)malloc(sizeof(number));
	if(intal==NULL)
		return NULL; 
	while(j<n-1 && str[j]=='0')	// to strip leading zeroes
		j++; 
	k = j;
	x = k;
	while(x<n && str[x]>='0' && str[x]<='9')
		x++;
	intal->num = (char*)malloc(x-k+1);
	intal->size = x-k+1;

	if(x==k){
		intal->num[0] = 0;
		intal->len = 1;
		return intal;
	}
	for(int i=x-k-1;i>=0 && j<x;i--,j++){
		intal->num[i] = str[j]-'0';
		if(intal->num[i]>9 || intal->num[i]<0){
			free(intal->num);
			free(intal);
			return NULL;
		}
	}
	intal->len = x-k;
	return intal;		
}

void intal_destroy(void* var){
	number *intal = (number*)var;
	if(intal == NULL)
		return;
	free(intal->num);
	free(var);
	var = NULL;
}


char* intal2str(void* var){
	number *intal = (number*)var;
	char *num;
	if(intal==NULL){
		num = (char*)malloc(4);
		strcpy(num, "NaN");
		return num;
	}
	int n = intal->len;
	num = (char*)malloc(n+1);
	for(int i=0,j=n-1;i<n,j>=0;i++,j--)
		num[j] = intal->num[i]+'0';
	num[n] = '\0';
	return num;
}

void* intal_increment(void* var){
	number *intal = (number*)var;
	char *newstr;
	int i,n;
	if(intal==NULL)
		return NULL;
	n = intal->len;
	i = 0;
	while(intal->num[i]==9 && i<n){
		intal->num[i]=0;
		i++;
	}
	if(i==n){
		if(n==intal->size){	// If the number of digits exceeds the size of the block
			newstr = (char*)realloc(intal->num, n+3);
			intal->num = newstr;
			intal->size = n+3;
		}
		intal->num[i] = 0;
		intal->len++;
	}
	intal->num[i]++;
	return intal;
}

void* intal_decrement(void* var){
	number *intal = (number*)var;
	int i,n;
	if(intal == NULL)
		return NULL;
	n = intal->len;
	i = 0;
	if(n==1 && intal->num[0]==0)	// If the number is zero the same number is returned
		return intal;
	while(intal->num[i]==0 && i<n){
		intal->num[i] = 9;
		i++;
	}
	intal->num[i]--;
	remove_zeroes(intal);
	return intal;
}

void* intal_add(void* var1, void* var2){
	number *intal1 = (number*)var1;
	number *intal2 = (number*)var2;
	int i,n,carry,len;
	number *sum = (number*)malloc(sizeof(number));
	if(sum==NULL)
		return NULL;
	sum->size = 0;
	len = intal1->len > intal2->len? intal1->len : intal2->len;	// Getting the size of the larger number 
	sum->num = (char*)malloc(len+2);
	sum->size = len+2;
	if(intal1==NULL || intal2==NULL)
		return NULL;
	n = intal1->len < intal2->len? intal1->len : intal2->len;
	carry = 0;							// setting the value of carry to zero initially
	i = 0;
	while(i<n && i < sum->size){
		sum->num[i] = intal1->num[i] + intal2->num[i] + carry;
		carry = sum->num[i]/10;
		sum->num[i] = sum->num[i]%10;
		i++;
	}
	while(i < intal1->len && i < sum->size){
		sum->num[i] = intal1->num[i] + carry;
		carry = sum->num[i]/10;
		sum->num[i] = sum->num[i]%10;
		i++;
	}
	while(i < intal2->len && i < sum->size){
		sum->num[i] = intal2->num[i] + carry;
		carry = sum->num[i]/10;
		sum->num[i] = sum->num[i]%10;
		i++;
	}
	if(carry>0){
		sum->num[i] = carry;
		i++;
	}
	sum->len = i;
	return sum;
}

void* intal_diff(void* var1, void* var2){
	number *intal1 = (number*)var1;
	number *intal2 = (number*)var2;
	int cmp,i,nb,ns,sub,borrow;					// nb and ns are length of the big and small respectively
	number *big=NULL,*small=NULL,*diff=NULL;
	if(intal1 == NULL || intal2 == NULL)
		return NULL;
	diff = (number*)malloc(sizeof(number));
	cmp = intal_compare(intal1,intal2);
	if(cmp==0){							// If the values are equal then the difference is zero
		diff->len = 1;
		diff->num = (char*)malloc(1);
		diff->num[0] = 0;
		return diff;
	}
	else if(cmp==1){
		big = intal1;
		small = intal2;
	}
	else{
		big = intal2;
		small = intal1;
	}
	nb = big->len;
	ns = small->len;
	i = 0;
	diff->num = (char*)malloc(nb+1);
	diff->size = nb+1;
	diff->num[0] = 0;
	while(i < ns && i < diff->size){
		sub = big->num[i] - small->num[i];	//Computing diffrence for that digit
		diff->num[i] += sub;
		if(diff->num[i] < 0 && i < diff->size){
			diff->num[i] += 10;	// Adding the current value with the difference
			diff->num[i+1] = -1;	// Accounting for borrow
		}
		else{
			diff->num[i+1] = 0;
		}
		i++;
	}
	while(i<nb && i < diff->size){
		diff->num[i] += big->num[i];
		if(diff->num[i] < 0){
			diff->num[i] += 10;
			diff->num[i+1] = -1;
		}
		else{
			diff->num[i+1] = 0;
		}
		i++;
	}
	diff->len = i;
	remove_zeroes(diff);
		
	return diff;
}

int intal_compare(void* var1, void* var2){
	number *intal1 = (number*)var1;
	number *intal2 = (number*)var2;
	int i,cmp;
	if(intal1==NULL || intal2==NULL)
		return -2;
	if(intal1->len > intal2->len)
		return 1;
	else if(intal1->len < intal2->len)
		return -1;
	for(i = intal1->len-1; i>=0; i--){	//loop continues until unequal digits are found
		if(intal1->num[i] > intal2->num[i])
			return 1;
		else if(intal1->num[i] < intal2->num[i])
			return -1;
	}
	return 0;
}



number** split(number* var1, int m){
	number *intal1 = (number*)var1;
	number *a1 = (number*)malloc(sizeof(number));
	a1->size = 0;	// Because it doesnot have a string of its own 
	number *a2 = (number*)malloc(sizeof(number));
	a2->size = 0;
	number **arr = (number**)malloc(sizeof(number*)*2);
	a1->len = a2->len = m;
	a1->num = intal1->num + m;
	a2->num = intal1->num;
	*(arr+0) = a2;
	*(arr+1) = a1;
	return arr;
}

number* karatsuba(number* var1, number* var2){
	int bigger,k,m,i,j,n;	// m is the split point 
	int flag = 0;	// flag to check whether data has been overwritten
	char prod,old1=0,old2=0;	// the older digits if its been ovewritten

	number *a1,*a2,*b1,*b2;	// The paritions of the number
	number **arr1,**arr2;	// The array that contains the partitions 
	
	number *e1,*e2;	// to simulate multiplication by 10^n and 10^n/2 respectively
	
	number *res,*p1,*p2,*p3,*sumA,*sumB;
	number *diff31, *diff312;	// This is to perform p3-p2-p1
	
	number *add2, *add1;	// This is to add p2 and p1 respectively
	number *tmpSumA, *tmpSumB;

	number *intal1 = (number*)var1;
	number *intal2 = (number*)var2;


	i = intal1->len;
	while(i < intal2->len && i < intal1->size){
		intal1->num[i] = 0;
		i++;
	}
	intal1->len = i;
	i = intal2->len;
	while(i < intal1->len && i < intal2->size){
		intal2->num[i] = 0;
		i++;
	}
	intal2->len = i;

	n = intal1->len; 
	res = (number*)malloc(sizeof(number));

	res->num = (char*)malloc(2*n+1);
	res->size = 2*n+1;
	res->len = 0;	// As a precaution
	if(n<=1){
		prod = intal1->num[0]*intal2->num[0];
		if(prod > 9){
			res->num[0] = prod%10;
			res->num[1] = prod/10;
			res->len = 2;
			return res;
		}
		res->num[0] = prod;
		res->len = 1;
		return res;
	}
	else if(n%2 == 1){	// padding with zeroes if the number of digits is odd
		old1 = intal1->num[n];
		intal1->num[n] = 0;
		intal1->len++;
		old2 = intal2->num[n];
		intal2->num[n] = 0;
		intal2->len++;
		n++;
		flag = 1;
	}
	m = n/2;
	arr1 = split(intal1, m);
	arr2 = split(intal2, m);
 
	a1 = arr1[1];
	a2 = arr1[0];	// LSDs in arr[0] because of little endian representation 
	b1 = arr2[1];
	b2 = arr2[0];

	p1 = karatsuba(a1,b1);
	p2 = karatsuba(a2,b2);
	

	tmpSumA = (number*)intal_add((void*)a1,(void*)a2);
	tmpSumB = (number*)intal_add((void*)b1,(void*)b2);
	
	bigger = tmpSumA->size > tmpSumB->size? 2*tmpSumA->size : 2*tmpSumB->size;

	sumA = (number*)malloc(sizeof(number));
	sumB = (number*)malloc(sizeof(number));
	sumA->num = (char*)malloc(bigger+2);
	sumA->size = bigger+2;
	sumB->num = (char*)malloc(bigger+2);
	sumB->size = bigger+2;
	for(int x = 0; x < tmpSumA->len; x++)
		sumA->num[x] = tmpSumA->num[x];
	sumA->len = tmpSumA->len;
	for(int x = 0; x < tmpSumB->len; x++)
		sumB->num[x] = tmpSumB->num[x];
	sumB->len = tmpSumB->len;
	intal_destroy(tmpSumA);
	intal_destroy(tmpSumB);

	p3 = karatsuba(sumA,sumB);
	
	free(a1);	// freeing the unnecessary values
	free(a2);
	free(b1);
	free(b2);
	free(arr1);
	free(arr2);
	intal_destroy(sumA);
	intal_destroy(sumB);

	
	remove_zeroes(p1);
	remove_zeroes(p2);
	remove_zeroes(p3);

	diff31 = (number*)intal_diff((void*)p3,(void*)p1);
	
	diff312 = (number*)intal_diff((void*)diff31,(void*)p2);
	e2 = (number*)malloc(sizeof(number));
	e2->size = 0;
	e2->len = p2->len > m && p2->len <= p2->size? p2->len - m : 0;
	e2->num = p2->num + m ;
	add2 = (number*)intal_add((void*)e2,(void*)diff312);
	
	e1 = (number*)malloc(sizeof(number));
	e1->size = 0;
	e1->len = add2->len > m && add2->len <= add2->size? add2->len - m: 0;
	e1->num = add2->num + m ;
	
	add1 = (number*)intal_add((void*)e1,(void*) p1);
	
	for(i=0;i<m && i< p2->len && i < res->size;i++)
		res->num[i] = p2->num[i]; 
	for(;i<m;i++)
		res->num[i] = 0;
	for(j=0;i<n && j<add2->len && j < res->size;i++,j++)
		res->num[i] = add2->num[j];
	for(;i<n;i++)
		res->num[i] = 0;
	for(j=0;j < add1->len && j < res->size;i++,j++)
		res->num[i] = add1->num[j];
	res->len = i;
	remove_zeroes(res);	//Removing leading zeroes

	// Destroying variables	
	intal_destroy(diff31);
	intal_destroy(diff312);

	intal_destroy(add1);
	intal_destroy(add2);
	intal_destroy(p1);
	intal_destroy(p2);
	intal_destroy(p3);
	free(e1);
	free(e2);
	e1 = NULL;
	e2 = NULL;
	
	if(flag==1){
		n--;
		intal1->num[n] = old1;
		intal2->num[n] = old2;
		intal1->len--;
		intal2->len--;
	}
	return res;
}


void* intal_multiply(void* var1, void* var2){
	int i;
	number *arg1 = (number*)var1;
	number *arg2 = (number*)var2;
	number *intal1, *intal2;
	number *res;

	int len = arg1->len > arg2->len? arg1->len : arg2->len;
	len = len%2 == 0? len: len + 1;		//making the length of the number even
	intal1 = (number*)malloc(sizeof(number));
	intal1->size = 0;
	intal2 = (number*)malloc(sizeof(number));
	intal2->size = 0;

	intal1->num = (char*)malloc(2*len);
	intal1->size = 2*len;
	intal2->num = (char*)malloc(2*len);
	intal2->size = 2*len;
	for(i=0;i<arg1->len && i<intal1->size;i++)		
		intal1->num[i] = arg1->num[i];
	for(;i<intal1->size;i++)		//padding with zeroes
		intal1->num[i] = 0;
	intal1->len = len;
	for(i=0;i<arg2->len && i < intal2->size;i++)
		intal2->num[i] = arg2->num[i];
	for(;i < intal2->size;i++)
		intal2->num[i] = 0;
	intal2->len = len;
	res = karatsuba(intal1, intal2);
	
	intal_destroy(intal1);
	intal_destroy(intal2);
	return res;
}


void* intal_divide(void* var1, void* var2){
	int cmp,n1,n2,len,i,k,j;	// i and j are loop variables
	number *substr = NULL;
	number *remainder = NULL;
	number *temp = NULL;	
	number *buffer = NULL;	// buffer containing part of the dividend 
	number *intal1 = (number*)var1;
	number *intal2 = (number*)var2;
	number *res = NULL;	// Quotient 
	if(intal1==NULL || intal2==NULL)
		return NULL;
	if(intal2->len == 1 && intal2->num[0] == 0)
		return NULL;
	res = (number*)malloc(sizeof(number));
	res->size = 0;
	cmp = intal_compare((void*)intal1,(void*)intal2);	//Comparing the two numbers
	if(cmp==0){
		res->num = (char*)malloc(2);
		res->size = 2;
		res->num[0] = 1;
		res->len = 1;
		return res;
	}
	else if (cmp==-1){
		res->num = (char*)malloc(2);
		res->size = 2;
		res->num[0] = 0;
		res->len = 1;
		return res;
	}
	len = intal1->len;	// length of the dividend 
	n1 = len;
	n2 = intal2->len;
	buffer = (number*)malloc(sizeof(number));
	substr = (number*)malloc(sizeof(number));
	substr->size = 0;
	
	buffer->num = (char*)malloc(n1+1);
	buffer->size = n1+1;
	res->num = (char*)malloc(n1+1);	
	res->size = n1+1;
	res->len = n1;
	for(i=0;i<n1;i++)
		buffer->num[i] = intal1->num[i];
	buffer->len = n1;
	substr->num = buffer->num + n1 - 1;
	substr->len = 1;
	i = n1-1;	// loop counter 
	while(i>=0){
		for(;i>=0 && substr->len < n2;i--){
			substr->num--;
			substr->len++;
			remove_zeroes(substr);
			res->num[i] = 0; 
		}
		remove_zeroes(substr);
		while(intal_compare((void*)substr, (void*)intal2)==-1 && i>=0){
			substr->num--;
			substr->len++;
			remove_zeroes(substr);
			res->num[i] = 0;
			i--;
		}
		if(i<0){
			break;
		}			
		remainder = intal_diff((void*)substr, (void*)intal2);
		res->num[i] = 1;
		while(intal_compare((void*)remainder, (void*)intal2)!=-1){
			temp = intal_diff((void*)remainder, (void*)intal2);	
			intal_destroy(remainder);
			remainder = temp;
			res->num[i]++;
		}
		for(j=0;j<remainder->len;j++){
			substr->num[j] = remainder->num[j];
		}
		intal_destroy(remainder);
		if(i==0)	// if we have reached the end of the number
			break;
		substr->num--;	// Moving to the next number
		substr->len = j+1;
		remove_zeroes(substr);	//Removing leading zeroes
		i--;
	}	
	remove_zeroes(res);

	intal_destroy(buffer);
	free(substr);
	substr = NULL;
	return res;
}

void* intal_pow(void* var1, void* var2){

	number *intal1 = (number*)var1;
	number *intal2 = (number*)var2;
	number *res=NULL;
	number *res2=NULL;
	number *res3=NULL;
	number *two=NULL;
	number *half=NULL;
	static int intal_pow_iter = 0;
	if(intal1==NULL || intal2==NULL)
		return NULL;
	if(intal2->len==1 && intal2->num[0]==0){
		res = (number*)malloc(sizeof(number));
		res->num = (char*)malloc(2);
		res->size = 2;
		res->num[0] = 1;
		res->len = 1;
		return res;
	}

	two = (number*)malloc(sizeof(number));	// Creating the number 2 
	two->num = (char*)malloc(1);
	two->size = 1;
	two->num[0] = 2;
	two->len = 1;
	half = intal_divide(intal2, two);
	
	intal_pow_iter++;

	res = intal_pow((void*)intal1, (void*)half);
	res2 = intal_multiply((void*)res, (void*)res);
 
	if(intal2->num[0]%2==1){
		res3 = intal_multiply(res2, intal1);
	}
	// Destroying unnecessary variables
	intal_destroy(res);
	intal_destroy(two);
	intal_destroy(half);
	if(res3!=NULL){
		res = res3;
		intal_destroy(res2);
	}
	else{
		res = res2;
	}
	return res;

}
