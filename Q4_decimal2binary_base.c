#include <stdio.h>
#include <math.h>

// function to convert decimal to binary 
void Decimal2Binary(unsigned char n) 
{ 
    // array to store binary number 
    unsigned char binaryNum[32]; 
      // counter for binary array 
    unsigned char i = 0; 

    printf("Binary format before reversal is: "); 

    // iteration when the quotient (updated number) not 0
    while (n > 0) {
    //// Please do not change the codes above ////     
        //// Please add your codes below //// 
        
        // storing remainder in binary array binaryNum[ ]           
        binaryNum[i] = n % 2;
        // then update the quotient value in each loop
        n = n / 2;
        // Print out the value binaryNum[] in each loop        
        printf("%d", binaryNum[i]);
        // increase the loop variable in each loop           
        i++;
        
        //// Please do not change the codes below ////        
    } 

    printf("\nReverse Order = "); 

    // printing binary array in reverse order 
    for (int j = i - 1; j >= 0; j--) 
        printf("%d", binaryNum[j]);
    printf("\n"); 
} 


//// Please do not change the codes below ////  
// main function 
int main() 
{ 
    int input = 0;
    printf("Enter the integer decimal to be converted into binary: ");
    scanf("%d", &input);   
    
    // call the conversion function
    Decimal2Binary(input); 
    return 0; 
} 
